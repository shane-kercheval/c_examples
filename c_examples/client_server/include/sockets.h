#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>


/**
 * @brief Creates a socket and connects it to a given IP address and port.
 * 
 * @param ip_address The IP address of the server.
 * @param port The port of the server.
 * @return The socket file descriptor of the server connection, or -1 if the connection fails.
 */
int connect_socket(const char* ip_address, in_addr_t port);

/**
 * @brief Creates a socket and connects it to a given IP address and port. Exits the program if the connection fails.
 * 
 * @param ip_address The IP address of the server.
 * @param port The port of the server.
 * @return The socket file descriptor of the server connection.
 */
int connect_or_die(const char* ip_address, in_addr_t port);

/**
 * @brief Creates a socket and connects it to a given IP address and port. Retries the connection if it fails. Exits the program if the connection fails after the maximum number of retries.
 * 
 * @param ip_address The IP address of the server.
 * @param port The port of the server.
 * @param max_retries The maximum number of retries.
 * @param retry_delay The delay between retries in seconds.
 * @return The socket file descriptor of the server connection.
 */
int connect_with_retry_or_die(const char* ip_address, in_addr_t port, int max_retries, int retry_delay);

/**
 * @brief Creates a socket and binds to a port. Exits the program if the binding fails.
 * 
 * @param port The port to bind to.
 * @return The socket file descriptor of the bound server socket/port.
 */
int bind_or_die(in_addr_t port);

/**
 * @brief Listens on a socket. Exits the program if listening fails.
 * 
 * @param server_socket The socket file descriptor to listen on.
 * @param backlog From the man page of listen(): The backlog parameter defines the maximum length for the queue of pending connections.
 */
void listen_or_die(int server_socket, int backlog);

/**
 * @brief Accepts a connection on a server socket. Exits the program if the connection fails.
 * 
 * @param server_socket The server socket file descriptor.
 * @return The client socket file descriptor of the accepted connection.
 */
int accept_or_die(int server_socket);

/**
 * @brief Receives data from a socket. Exits the program if the receive fails.
 * 
 * @param socket_fd The socket file descriptor to receive data from.
 * @param buffer The buffer to store the received data.
 * @param length The length of the buffer.
 */
ssize_t receive_or_die(int socket_fd, void* buffer, size_t length);


/**
 * @brief cleans up the socket file descriptor (shutdown and close).
 */
void socket_cleanup(int socket_fd);

#endif // SOCKETS_H
