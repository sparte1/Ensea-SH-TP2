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
    
    // Step 5: Cleanup and close resources
    freeaddrinfo(res);
    close(sockfd);

    return 0; // Exit successfully
}
