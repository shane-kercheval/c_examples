#include "utils.h"
#include "protocol.h"
#include "file_transfer.h"
#include <sockets.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#define PORT 9002

/**
 * @brief accept a connection and return the client socket, or -1 if the connection fails.
 */
int accept_connection(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    return accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
}

int main() {
    printf("\n\nServer started\n");
    int server_socket = bind_or_die(PORT);
    printf("Server bound to port %d\n", PORT);
    listen_or_die(server_socket, 1);
    while (1) {
        printf("\n---------\nWaiting for connection\n");
        int client_socket = accept_connection(server_socket);
        if (client_socket == -1) {
            fprintf(stderr, "***ERROR*** accepting connection\n");
            socket_cleanup(client_socket);
            continue;
        }
        printf("Connection accepted\n");
        uint8_t buffer[MAX_MESSAGE_SIZE];
        ssize_t bytes_received = recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        if (bytes_received <= 0) {
            fprintf(stderr, "***ERROR*** receiving message\n");
            socket_cleanup(client_socket);
            continue;
        }
        Response response;
        int status = parse_message(buffer, bytes_received, &response);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error parsing message\n");
            socket_cleanup(client_socket);
            continue;
        }
        printf("Received request: command=%d, payload=%s\n", response.header.command, (char*)response.payload);
        status = handle_request(client_socket, &response.header, response.payload);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error handling request: status=%d\n", status);
            socket_cleanup(client_socket);
            continue;
        }
        else {
            printf("Request handled\n");
        
        }
        socket_cleanup(client_socket);
        destroy_response(&response);
    }
    socket_cleanup(server_socket);
    return 0;
}
