#include <unistd.h>   
#include <stdlib.h>  
#include <string.h>   
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

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
    if (argc != 3) {  
        if (strcmp(operation, "gettftp") == 0) {
            write_stderr_and_exit("Usage: gettftp <server> <file>\n");
        } else if (strcmp(operation, "puttftp") == 0) {
            write_stderr_and_exit("Usage: puttftp <server> <file>\n");
        } else {
            write_stderr_and_exit("Invalid operation. Use 'gettftp' or 'puttftp'.\n");
        }
    }
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
	} else if (strcmp(operation, "puttftp") == 0) {
        write_stdout("Preparing to PUT file...\n");
        write_stdout("Uploading to server: ");
        write_stdout(server);
        write_stdout("\nFile to upload: ");
        write_stdout(file);
        write_stdout("\n");
    } else {
        write_stderr_and_exit("Invalid operation. Use 'gettftp' or 'puttftp'.\n");
    }
}

int main(int argc, char *argv[]) {
	// Extract the operation (gettftp or puttftp)
    const char *operation = argv[0]; 
    validate_args(argc, operation);

    // Extract server and file
    const char *server = argv[1];
    const char *file = argv[2];
    
    perform_operation(operation, server, file);

    return 0;
}
