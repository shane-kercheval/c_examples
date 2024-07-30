// https://www.youtube.com/watch?v=mStnzIEprH8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DEFAULT_PROTOCOL 0
#define RESPONSE_SIZE 4096


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }
    char *address = argv[1];
    // convert third argument (port number) to integer
    int port = atoi(argv[2]);  // ascii to integer

    // Despite the name, it's not a socket for receiving data at the client end; it's the socket
    // through which the client sends data to and receives data from the server.
    // We will connect this socket to the server which means this is a mechanism for the client to
    // communicate with the server.
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (client_socket_fd == -1) {
        perror("socket");
        exit(1);
    }
    // now that we have a socket we need to connect it to the other side i.e. the server
    // now let's use the address structure from netinet/in.h so that we can specify the address
    // of the server
    struct sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;  // specify we are using the internet protocol
    // we need to convert the port number to "network byte order" using the htons function
    // which stands for "host to network short"
    remote_address.sin_port = htons(port);
    // remote_address.sin_addr.s_addr = INADDR_ANY;  // same as connecting to localhost, or 0.0.0.0
    // inet_aton stands for "internet address to number" and converts the address to binary form
    inet_aton(address, &remote_address.sin_addr);  // convert the address to binary form
    // connect to the server
    int connection_status = connect(
        client_socket_fd,
        (struct sockaddr *) &remote_address, 
        sizeof(remote_address)
    );
    if (connection_status == -1) {
        printf("Connection failed.\n");
        perror("connect");
        exit(1);
    }
    // was getting 400 from server, seems like because I didn't have host in the request
    // char request[] = "GET / HTTP/1.1\r\n\r\n";
    char request[] = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    char server_response[RESPONSE_SIZE] = {0}; 
    send(client_socket_fd, request, strlen(request), 0);
    recv(client_socket_fd, &server_response, sizeof(server_response), 0);
    // 0 is a flag that indicates no special flags are set
    recv(client_socket_fd, &server_response, sizeof(server_response), 0);
    printf("The server sent the data: %s\n", server_response);
    close(client_socket_fd);
    return 0;
}
