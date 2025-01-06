#include <unistd.h>   
#include <stdlib.h>  
#include <string.h>   
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>

void send_file_data(int sock, const char *filename, struct sockaddr *server_addr, socklen_t server_addr_len, uint16_t server_port);

// Validate command-line arguments: Expect exactly 3 arguments (operation, server, file)
void validate_args(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <domain> <file>\n", argv[0]);
        exit(EXIT_FAILURE); // Exit if the arguments are invalid
    }
}

// Resolve the server address using getaddrinfo
struct addrinfo* resolve_server(const char *domain, const char *port) {
    struct addrinfo hints, *res;

    // Initialize the hints structure
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // Use IPv4
    hints.ai_socktype = SOCK_DGRAM;  // Use UDP for TFTP
    
    // Call getaddrinfo to resolve the server's address
    int status = getaddrinfo(domain, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "Error in getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE); // Exit on failure
    }

    return res; // Return the resolved address (caller must free it)
}

// Function to create and return a socket descriptor
int create_socket(struct addrinfo *res) {
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

// Function to send a WRQ (Write Request) to the server
void send_wrq(int sock, const char *filename, struct addrinfo *res) {
    char mode[] = "octet"; // Transfer mode
    size_t wrq_len = 2 + strlen(filename) + 1 + strlen(mode) + 1;
    char *wrq = malloc(wrq_len);
    if (!wrq) {
        perror("Memory allocation error for WRQ");
        close(sock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    
    memset(wrq, 0, wrq_len);
    wrq[0] = 0x00; // Opcode for WRQ
    wrq[1] = 0x02; // Write request
    strcpy(wrq + 2, filename); // Add filename
    strcpy(wrq + 2 + strlen(filename) + 1, mode); // Add transfer mode
    
    // Send the data to the server
    if (sendto(sock, wrq, wrq_len, 0, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Error sending WRQ");
        free(wrq);
        close(sock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    
    fprintf(stdout, "WRQ sent for file '%s'. Waiting for ACK 0...\n", filename);
    free(wrq);
    
    // Wait for ACK 0
    char ack[4];
    struct sockaddr_in server_ack_addr; // Structure to hold the server's address from ACK
    socklen_t server_addr_len = sizeof(server_ack_addr);
    ssize_t ack_size = recvfrom(sock, ack, sizeof(ack), 0, (struct sockaddr *)&server_ack_addr, &server_addr_len);
    if (ack_size < 0) {
        perror("Error receiving ACK 0");
        close(sock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    
    // Verify that the acknowledgment is ACK 0
    if (ack[0] != 0x00 || ack[1] != 0x04 || ack[2] != 0x00 || ack[3] != 0x00) {
        fprintf(stderr, "Invalid ACK received. Expected ACK 0.\n");
        close(sock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "ACK 0 received. Ready to send file data.\n");
    
    // Extract the server's source port from ACK 0
    uint16_t server_port = ntohs(server_ack_addr.sin_port); // The port from where ACK was received
    fprintf(stdout, "Server is using port %d for data transfer.\n", server_port);

    // Send file data using the extracted server port
    send_file_data(sock, filename, (struct sockaddr *)&server_ack_addr, server_addr_len, server_port);
}
		
// Function to send file data in TFTP DAT packets
void send_file_data(int sock, const char *filename, struct sockaddr *server_addr, socklen_t server_addr_len, uint16_t server_port) {
    FILE *infile = fopen(filename, "rb");
    if (!infile) {
        perror("Error opening file");
        close(sock);
        exit(EXIT_FAILURE);
    }        
    
    char buffer[516]; // 512 bytes data + 4 bytes header
    char ack[4];      // Buffer for acknowledgment
    size_t block_number = 1; // TFTP block numbers start from 1
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer + 4, 1, 512, infile)) > 0) {
        // Construct the data packet
        buffer[0] = 0x00;
        buffer[1] = 0x03; // DATA opcode
        buffer[2] = (block_number >> 8) & 0xFF; // Block number high byte
        buffer[3] = block_number & 0xFF;        // Block number low byte
        
        // Send the data packet
        if (sendto(sock, buffer, bytes_read + 4, 0, server_addr, server_addr_len) < 0) {
            perror("Error sending data packet");
            fclose(infile);
            close(sock);
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "Data block %zu sent (%zu bytes).\n", block_number, bytes_read);

		// Receive acknowledgment for the block
        ssize_t ack_size = recvfrom(sock, ack, sizeof(ack), 0, server_addr, &server_addr_len);
        if (ack_size < 0) {
            perror("Error receiving acknowledgment");
            fclose(infile);
            close(sock);
            exit(EXIT_FAILURE);
        }
        
        // After receiving acknowledgment for the block
		fprintf(stdout, "Received ACK: %02x %02x %02x %02x\n", ack[0], ack[1], ack[2], ack[3]);
        
        // Verify acknowledgment opcode and block number
		if (ack[1] != 0x04 || ack[2] != ((block_number >> 8) & 0xFF) || ack[3] != (block_number & 0xFF)) {
			fprintf(stderr, "Invalid acknowledgment received for block %zu. Expected ACK: 0x00 0x04 0x%02lx 0x%02lx\n",
				block_number, (block_number >> 8) & 0xFF, block_number & 0xFF);
			fclose(infile);
			close(sock);
			exit(EXIT_FAILURE);
		}
		
        block_number++;
    }

    fprintf(stdout, "File transfer complete.\n");
    fclose(infile);
}

int main(int argc, char *argv[]) {
    // Step 1: Validate the command-line arguments
    validate_args(argc, argv);

    // Step 2: Extract the domain and file name from arguments
    char *domain = argv[1];  // Server's address (e.g., "127.0.0.1")
    char *filename = argv[2]; // Name of the file to download
    char *port = "1069";        // Default TFTP Port

    // Step 3: Resolve the server's address
    struct addrinfo *res = resolve_server(domain, port);
    
    // Step 4: Create a socket
    int sockfd = create_socket(res);
    
    // Step 5: Send WRQ and send data file
    send_wrq(sockfd, filename, res);
    
    // Step 6: Cleanup and close resources
    freeaddrinfo(res);
    close(sockfd);

    return 0; // Exit successfully
}
