// https://www.youtube.com/watch?v=LtXEMwSG5-8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

# define DEFAULT_PROTOCOL 0
# define PORT 9002
# define BACKLOG 10 
// backlog is the number of connections allowed on the incoming queue. What does that mean?
// Well, incoming connections are going to wait in this queue until you accept() them and this is
// the limit on how many can queue up. Most systems silently limit this number to about 20; you can
// probably get away with setting it to 5 or 10.
// define is a preprocessor directive that defines a macro. Macros are used to define constants.
// When compiling, the preprocessor replaces the names of the macros with their values.


int main() {
    // allocates space for 256 characters which can be overwritten
    // char server_message[256] = "You have reached the server!";
    // This uses a string literal which is typically stored in read-only memory and is more memory
    // efficient since it does not allocate extra space
    char *server_message = "You have reached the server!";
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (server_socket_fd == -1) {
        perror("socket");
        exit(1);
    }
    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    // bind the socket to our specified IP and port
    if (bind(server_socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("bind");
        exit(1);
    }
    // listen for connections
    listen(server_socket_fd, BACKLOG);
    // accept a connection
    int client_socket_fd = accept(server_socket_fd, NULL, NULL);
    if (client_socket_fd == -1) {
        perror("accept");
        exit(1);
    }
    // send the message
    // 0 is a flag that indicates no special flags are set
    send(client_socket_fd, server_message, strlen(server_message), 0);
    // close the socket
    close(server_socket_fd);
    return 0;
}