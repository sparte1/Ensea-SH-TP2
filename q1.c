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

int main(int argc, char *argv[]) {
	// Step 1: Validate the command-line arguments
    validate_args(argc);

    // Step 2: Extract the domain and file name from arguments
    char *domain = argv[1];  // Server's address (e.g., "127.0.0.1")
    char *filename = argv[2]; // Name of the file to download
    char *port = "69";        // Default TFTP Port
    
    return 0;
}
