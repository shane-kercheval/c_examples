#include "sockets.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// adopted from:
// https://beej.us/guide/bgnet/source/examples/client.c
// https://beej.us/guide/bgnet/source/examples/server.c
// https://www.youtube.com/watch?v=LtXEMwSG5-8 - Socket Programming Tutorial In C For Beginners | Part 1 | Eduonix
// https://www.youtube.com/watch?v=mStnzIEprH8 - Socket Programming Tutorial In C For Beginners | Part 2 | Eduonix

int connect_socket(const char* ip_address, in_addr_t port) {
    // AF_INET: IPv4; SOCK_STREAM: TCP; 0: default protocol
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);  // host to network short
    // address.sin_addr.s_addr = INADDR_ANY;
    // from man:  "The inet_pton() function converts a presentation format address (that is,
    // printable form as held in a character string) to network format. It returns 1 if the address
    // was valid for the specified address family, or 0 if the address was not parseable in the
    // specified address family, or -1 if some system error occurred (in which case errno will have
    // been set).  This function is presently valid for AF_INET and AF_INET6.
    int status = inet_pton(AF_INET, ip_address, &address.sin_addr);
    if (status <= 0) {
        perror("inet_aton");
        socket_cleanup(server_socket);
        exit(1);
    }
    status = connect(server_socket, (struct sockaddr *)&address, sizeof(address));
    if (status == -1) {
        return -1;
    }
    return server_socket;
}

int connect_or_die(const char* ip_address, in_addr_t port) {
    int server_socket = connect_socket(ip_address, port);
    if (server_socket == -1) {
        perror("connect");
        socket_cleanup(server_socket);
        exit(1);
    }
    return server_socket;
}

int connect_with_retry_or_die(const char* ip_address, in_addr_t port, int max_retries, int retry_delay) {
    // AF_INET: IPv4; SOCK_STREAM: TCP; 0: default protocol
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);  // host to network short
    // address.sin_addr.s_addr = INADDR_ANY;
    // from man:  "The inet_pton() function converts a presentation format address (that is,
    // printable form as held in a character string) to network format. It returns 1 if the address
    // was valid for the specified address family, or 0 if the address was not parseable in the
    // specified address family, or -1 if some system error occurred (in which case errno will have
    // been set).  This function is presently valid for AF_INET and AF_INET6.
    int status = inet_pton(AF_INET, ip_address, &address.sin_addr);
    if (status <= 0) {
        perror("inet_aton");
        goto error_close_socket;
    }
    for (int i = 0; i < max_retries; i++) {
        if (connect(server_socket, (struct sockaddr *)&address, sizeof(address)) == 0) {
            return server_socket;
        }
        // else status is -1
        // if the connection was refused, sleep for a while and try again
        // otherwise, print the error and exit
        if (errno == ECONNREFUSED) {
            sleep(retry_delay);
        }
        else {
            perror("connect");
            goto error_close_socket;
        }
    }
    perror("connect");

error_close_socket:
    socket_cleanup(server_socket);
    exit(1);
}

int bind_or_die(in_addr_t port) {
    // AF_INET: IPv4; SOCK_STREAM: TCP; 0: default protocol
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }
    // we need to force the reuse of the address; otherwise restarting immediately after a crash
    // or unit tests will fail with "bind: Address already in use"
    int option_value = 1;
    // from man page:
    // SO_REUSEADDR: enables local address reuse
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value)) == -1) {
        perror("setsockopt");
        socket_cleanup(server_socket);
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);  // host to network short
    address.sin_addr.s_addr = INADDR_ANY;
    int status = bind(server_socket, (struct sockaddr *)&address, sizeof(address));
    if (status == -1) {
        perror("bind");
        socket_cleanup(server_socket);
        exit(1);
    }
    return server_socket;
}

void listen_or_die(int server_socket, int backlog) {
    int status = listen(server_socket, backlog);
    if (status == -1) {
        perror("listen");
        socket_cleanup(server_socket);
        exit(1);
    }
}

int accept_or_die(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("accept");
        socket_cleanup(server_socket);
        exit(1);
    }
    return client_socket;
}

ssize_t receive_or_die(int socket_fd, void* buffer, size_t length) {
    ssize_t bytes_received = recv(socket_fd, buffer, length, 0);
    if (bytes_received <= 0) {
        perror("recv");
        socket_cleanup(socket_fd);
        exit(1);
    }
    return bytes_received;
}

void socket_cleanup(int socket_fd) {
    if (socket_fd != -1) {
        shutdown(socket_fd, SHUT_RDWR);
        close(socket_fd);
    }
}
