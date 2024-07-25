#include "sockets.h"
#include "protocol.h"
#include "file_transfer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT 9002
#define ADDRESS "0.0.0.0"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <command> <file_name>\n", argv[0]);
        return 1;
    }
    int command = atoi(argv[1]);
    const char* file_name = argv[2];
    
    int server_socket;
    int rvalue;
    Response response;
    switch (command) {
        case 0:
            printf("\n\nRequesting File Metadata: `%s`\n", file_name);
            server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
            rvalue = request_file_metadata(server_socket, file_name, &response);
            socket_cleanup(server_socket);
            if (rvalue != STATUS_OK) {
                printf("Error requesting file metadata: `%d`==`%d`?\n", rvalue, response.header.status);
                printf("Error message: %s\n", (char*) response.payload);
                return 1;
            }
            printf("Received metadata for file `%s` - `%s`\n\n", file_name, (char*) response.payload);
            destroy_response(&response);
            break;
        case 1:
            printf("\n\nRequesting File Contents: `%s`\n", file_name);
            server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
            rvalue = request_file_contents(server_socket, file_name, &response);
            socket_cleanup(server_socket);
            if (rvalue != STATUS_OK) {
                printf("Error requesting file contents: `%d`==`%d`?\n", rvalue, response.header.status);
                printf("Error message: %s\n", (char*) response.payload);
                return 1;
            }
            printf("Payload size: %d\n", response.header.payload_size);
            printf("Number of chunks: %d\n", response.header.chunk_index + 1);
            printf("Received contents for file `%s`:\n", file_name);
            printf("Contents:\n------\n%s\n------\n\n", (char*) response.payload);
            destroy_response(&response);
            break;
            break;
        default:
            printf("Unknown command\n");
            break;
    }
    return 0;
}
