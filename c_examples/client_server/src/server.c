#include "utils.h"
#include "protocol.h"
#include "file_transfer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sockets.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 9002

/**
 * @brief accept a connection and return the client socket, or -1 if the connection fails.
 */
int accept_connection(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    return accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
}

void* server_worker(void* arg) {
    // the client socket is stored on the heap to pass into the thread; don't forget to free it
    // convert it to an int pointer and then dereference it to get the value
    int client_socket = *(int*)arg;
    free(arg);

    uint8_t buffer[MAX_MESSAGE_SIZE];
    ssize_t bytes_received = recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
    if (bytes_received <= 0) {
        fprintf(stderr, "***ERROR*** receiving message\n");
        socket_cleanup(client_socket);
        return NULL;
    }

    Response response;
    int rvalue = parse_message(buffer, bytes_received, &response);
    if (rvalue != STATUS_OK) {
        fprintf(stderr, "Error parsing message\n");
        socket_cleanup(client_socket);
        return NULL;
    }
    printf("Received request (socket=%d): command=%d, payload=%s\n", client_socket, response.header.command, (char*)response.payload);
    rvalue = handle_request(client_socket, &response.header, response.payload);
    if (rvalue != STATUS_OK) {
        fprintf(stderr, "Error handling request: status=%d\n", rvalue);
    } else {
        printf("Request handled\n");
    }
    socket_cleanup(client_socket);
    destroy_response(&response);
    return NULL;
}

int main() {
    printf("\n\nServer started\n");
    int server_socket = bind_or_die(PORT);
    printf("Server bound to port %d\n", PORT);
    listen_or_die(server_socket, 1);
    // for each connection, we are going to create a new thread to handle it
    while (1) {
        printf("\n---------\nWaiting for connection\n");
        int* client_socket = malloc(sizeof(int));
        *client_socket = accept_connection(server_socket);
        if (*client_socket == -1) {
            fprintf(stderr, "***ERROR*** accepting connection\n");
            free(client_socket);
            continue;
        }
        printf("Connection accepted\n");
        pthread_t thread;
        if (pthread_create(&thread, NULL, server_worker, client_socket) != 0) {
            fprintf(stderr, "***ERROR*** creating thread\n");
            socket_cleanup(*client_socket);
            free(client_socket);
        } else {
            // from man page:
            // "The pthread_detach() function is used to indicate to the implementation that storage
            // for the thread thread can be reclaimed when the thread terminates. If thread has not
            // terminated, pthread_detach() will not cause it to terminate
            pthread_detach(thread);  // Detach the thread to reclaim resources after it finishes
        }
    }
    socket_cleanup(server_socket);
    return 0;
}
