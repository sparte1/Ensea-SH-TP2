#include <unistd.h>   
#include <stdlib.h>  
#include <string.h>   
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>


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

// Function to construct and send an RRQ (Read Request)
void send_rrq(int sock, const char *filename, struct addrinfo *res) {
    char mode[] = "octet";
    size_t rrq_len = 2 + strlen(filename) + 1 + strlen(mode) + 1;
    char *rrq = malloc(rrq_len);
    if (!rrq) {
        perror("Memory allocation error for RRQ");
        close(sock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    
    memset(rrq, 0, rrq_len);
    rrq[0] = 0x00;
    rrq[1] = 0x01;                      // RRQ opcode
    strcpy(rrq + 2, filename);          // Add filename
    strcpy(rrq + 2 + strlen(filename) + 1, mode); // Add transfer mode
    
    if (sendto(sock, rrq, rrq_len, 0, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Error sending RRQ");
        free(rrq);
        close(sock);
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "RRQ sent for file '%s'.\n", filename);
    free(rrq);
}

// Receive data from the server and write to a local file
void receive_and_process_data(int sockfd, const char *filename, struct sockaddr *serv_addr, socklen_t serv_addr_len) {
    // Open the local file for writing
    FILE *outfile = fopen(filename, "wb");
    if (!outfile) {
        perror("Error creating local file");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Buffer for receiving TFTP packets
    char buffer[516]; // 512 bytes data + 4 bytes header
    ssize_t bytes_received;
    
    while (1) {
        // Receive a data packet from the server
        bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, serv_addr, &serv_addr_len);
        if (bytes_received == -1) {
            perror("Error receiving data");
            fclose(outfile);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        
        // Check for DATA opcode (0x03)
        if (buffer[1] != 0x03) {
            fprintf(stderr, "Unexpected packet received (opcode: %d).\n", buffer[1]);
            fclose(outfile);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        
        // Write data to file (starting from 4th byte)
		fwrite(buffer + 4, 1, bytes_received - 4, outfile);
		fprintf(stdout, "Block 1 received and written to file '%s'.\n", filename);
        
        // Send ACK
		char ack[4] = {0x00, 0x04, buffer[2], buffer[3]}; // ACK with block number
		if (sendto(sock, ack, sizeof(ack), 0, server_addr, server_addr_len) < 0) {
			perror("Error sending ACK");
		} else {
			fprintf(stdout, "ACK sent for block 1.\n");
		}
	}

    // Close the file
    fclose(outfile);
}
        
        
int main(int argc, char *argv[]) {
    // Step 1: Validate the command-line arguments
    validate_args(argc);

    // Step 2: Extract the domain and file name from arguments
    char *domain = argv[1];  // Server's address (e.g., "127.0.0.1")
    char *filename = argv[2]; // Name of the file to download
    char *port = "1069";        // Default TFTP Port

    // Step 3: Resolve the server's address
    struct addrinfo *res = resolve_server(domain, port);
    
    // Step 4: Create a socket
    int sockfd = create_socket(res);
    
    // Step 5: Send RRQ
    send_rrq(sock, filename, res);
    
    // Step 6: Receive and process data from the server
    receive_and_process_data(sockfd, filename, res->ai_addr, res->ai_addrlen);
    
    // Step 7: Cleanup and close resources
    freeaddrinfo(res);
    close(sockfd);

    return 0; // Exit successfully
}
