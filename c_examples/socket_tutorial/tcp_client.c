// https://www.youtube.com/watch?v=LtXEMwSG5-8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DEFAULT_PROTOCOL 0
#define PORT 3490
#define BUFFER_SIZE 256


int main() {
    // create a socket
    // AF_INET is a constant defined in one of the libraries that means we are using the internet
    // protocol. It also means we are using an IPv4 address.
    // SOCK_STREAM means we are using a TCP connection (as opposed to a datagram/UDP connection).
    // 0 is the protocol. `0` means the default protocol for the chosen socket type which is TCP
    // for SOCK_STREAM).
    int network_socket_fd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (network_socket_fd == -1) {
        perror("socket");
        exit(1);
    }
    // now that we have a socket we need to connect it to the other side i.e. the server
    // now let's use the address structure from netinet/in.h so that we can specify the address
    // of the server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;  // specify we are using the internet protocol
    // we need to convert the port number to "network byte order" using the htons function
    // which stands for "host to network short"
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;  // same as connecting to localhost, or 0.0.0.0

    // connect to the server
    int connection_status = connect(
        network_socket_fd,
        (struct sockaddr *) &server_address, 
        sizeof(server_address)
    );
    if (connection_status == -1) {
        printf("Connection failed.\n");
        perror("connect");
        exit(1);
    }
    char server_response[BUFFER_SIZE];
    memset(server_response, 0, sizeof(server_response));  // NOLINT
    // 0 is a flag that indicates no special flags are set
    recv(network_socket_fd, &server_response, sizeof(server_response), 0);
    printf("The server sent the data: %s\n", server_response);

    close(network_socket_fd);
    return 0;
}
