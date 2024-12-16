#include <unistd.h>   
#include <stdlib.h>  
#include <string.h>   
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>


// Write a message to standard output
void write_stdout(const char *message) {
    if (write(STDOUT_FILENO, message, strlen(message)) == -1) {
        _exit(EXIT_FAILURE);
    }
}

// Write a message to standard error and exit
void write_stderr_and_exit(const char *message) {
    if (write(STDERR_FILENO, message, strlen(message)) == -1) {
        _exit(EXIT_FAILURE);
    }
    _exit(EXIT_FAILURE);
}

// Validate command-line arguments: Expect exactly 3 arguments (operation, server, file)
void validate_args(int argc, const char *operation) {
    if (argc != 4) {  // 4 for : ./q1 operation server file
        if (strcmp(operation, "gettftp") == 0) {
            write_stderr_and_exit("Usage: gettftp <server> <file>\n");
        } else if (strcmp(operation, "puttftp") == 0) {
            write_stderr_and_exit("Usage: puttftp <server> <file>\n");
        } else {
            write_stderr_and_exit("Invalid operation. Use 'gettftp' or 'puttftp'.\n");
        }
    }
}

// Resolve the server's address using getaddrinfo
struct addrinfo* resolve_server(const char *server) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    
	int ret = getaddrinfo(server, "1069", &hints, &res);
    if (ret != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		exit(EXIT_FAILURE);
	}

    return res; 
}

// Print the resolved server address
void print_resolved_address(struct addrinfo *res) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    char ipstr[INET_ADDRSTRLEN]; // For storing the IPv4 address

    // Convert the IPv4 address to a string
    inet_ntop(res->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
    write_stdout(" IPv4 adress : ");
    write_stdout(ipstr);
    write_stdout("\n");
}


// Perform the requested operation
void perform_operation(const char *operation, const char *server, const char *file) {
    if (strcmp(operation, "gettftp") == 0) {
        write_stdout("Preparing to GET file...\n");
        write_stdout("Downloading from server: ");
        write_stdout(server);
        write_stdout("\nFile to download: ");
        write_stdout(file);
        write_stdout("\n");
        
        struct addrinfo *res = resolve_server(server);
        print_resolved_address(res);
        freeaddrinfo(res);
	} else if (strcmp(operation, "puttftp") == 0) {
        write_stdout("Preparing to PUT file...\n");
        write_stdout("Uploading to server: ");
        write_stdout(server);
        write_stdout("\nFile to upload: ");
        write_stdout(file);
        write_stdout("\n");
        
        struct addrinfo *res = resolve_server(server);
        print_resolved_address(res);
        freeaddrinfo(res);
    } else {
        write_stderr_and_exit("Invalid operation. Use 'gettftp' or 'puttftp'.\n");
    }
}

int main(int argc, char *argv[]) {
	// Extract the operation (gettftp or puttftp)	
    const char *operation = argv[1]; 
    validate_args(argc, operation);
    
    // Extract server and file
    const char *server = argv[2];
    const char *file = argv[3];
    
    perform_operation(operation, server, file);

    return 0;
}
