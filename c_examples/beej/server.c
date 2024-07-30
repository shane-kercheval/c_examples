/*
** server.c -- a stream socket server demo
https://beej.us/guide/bgnet/examples/server.c

All this server does is send the string “Hello, world!” out over a stream connection. All you need
to do to test this server is run it in one window, and telnet to it from another with:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"  // the port users will be connecting to

//backlog is the number of connections allowed on the incoming queue. What does that mean?
// Well, incoming connections are going to wait in this queue until you accept() them and this is
// the limit on how many can queue up. Most systems silently limit this number to about 20; you can
// probably get away with setting it to 5 or 10.
// define is a preprocessor directive that defines a macro. Macros are used to define constants.
// When compiling, the preprocessor replaces the names of the macros with their values.
#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int socket_fd, client_fd;  // listen on socket_fd, new connection on client_fd
	struct addrinfo hints, *server_info = NULL, *p = NULL;
	struct sockaddr_storage client_address; // connector's address information
	socklen_t address_size;
	struct sigaction sa;
	int YES = 1;
	char ip_string[INET6_ADDRSTRLEN];
	int status;

    // ensure hints is emptied out. This is necessary because the hints structure is used to
    // specify criteria for selecting the socket address structures returned in the list pointed to
    // by res. If hints is not emptied out, the program may not work as expected because, in C, the
    // memory allocated for a variable is not zeroed out by default.
	memset(&hints, 0, sizeof hints);  // NOLINT
	hints.ai_family = AF_UNSPEC;  //AF_UNSPEC allow our code can be IP version-agnostic.
	hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // use local IP

    // NULL argument means that the IP address of the local host is used.
	if ((status = getaddrinfo(NULL, PORT, &hints, &server_info)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = server_info; p != NULL; p = p->ai_next) {
        // get a socket file descriptor
		if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
        // SOL_SOCKET indicates that the socket options are to be set at the socket level.
        // SO_REUSEADDR allows the socket to be bound to an address that is already in use.
        // This is useful for servers that bind to a specific address and port which might need
        // to be restarted while a previous connection is still in a timeout state.
        // YES is the value that is being set for the SO_REUSEADDR option.
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
        // server needs to bind to a port on the local machine so that clients can connect to it.
        // client would not call bind()
		if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("server: bind");
			continue;
		}

		break;
	}

    // ChatGPT:
    // When you call getaddrinfo(), it dynamically allocates memory for a linked list of addrinfo
    // structures, each containing information that can be used to create and configure a socket
    // for the specified host and service. This information includes IP addresses, port numbers,
    // and protocol types, among other details.  After you're done using the addrinfo linked list
    // (for example, after creating a socket and establishing a connection), it's important to free
    // the allocated memory to prevent memory leaks. This is what freeaddrinfo(server_info);
    // accomplishes. It ensures that all the memory allocated for the addrinfo structures in the
    // server_info list is properly released back to the system.
	freeaddrinfo(server_info); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
    // ChatGPT:
    // It marks the socket referred to by socket_fd as a passive socket, meaning it will be used to
    // accept incoming connection requests rather than initiate connections.
	if (listen(socket_fd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

    // Get ready—the accept() call is kinda weird! What’s going to happen is this: someone far far
    // away will try to connect() to your machine on a port that you are listen()ing on. Their
    // connection will be queued up waiting to be accept()ed. You call accept() and you tell it to
    // get the pending connection. It’ll return to you a brand new socket file descriptor to use
    // for this single connection! That’s right, suddenly you have two socket file descriptors for
    // the price of one! The original one is still listening for more new connections, and the
    // newly created one is finally ready to send() and recv().
    const char* message = "Hello, world!";
    ulong message_length = strlen(message);

	while(1) {  // main accept() loop
		address_size = sizeof(client_address);
        // client_address will usually be a pointer to a local struct sockaddr_storage. This is where
        // the information about the incoming connection will go (and with it you can determine
        // which host is calling you from which port). 
        // addrlen (address_size) is a local integer variable that should be set to
        // `sizeof(struct sockaddr_storage)` before its address is passed to accept(). accept()
        // will not put more than that many bytes into addr. If it puts fewer in, it’ll change the
        // value of addrlen to reflect that.
		client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &address_size);
		if (client_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(
            client_address.ss_family,
            get_in_addr((struct sockaddr *)&client_address), ip_string, sizeof(ip_string)
        );
		printf("server: got connection from %s\n", ip_string);
		if (!fork()) { // this is the child process
			close(socket_fd); // child doesn't need the listener
            // send() returns the number of bytes actually sent out—this might be less than the
            // number you told it to send! See, sometimes you tell it to send a whole gob of data
            // and it just can’t handle it. It’ll fire off as much of the data as it can, and trust
            // you to send the rest later. Remember, if the value returned by send() doesn’t match
            // the value in len, it’s up to you to send the rest of the string. 
			if (send(client_fd, message, message_length, 0) == -1)
				perror("send");
			close(client_fd);
			exit(0);
		}
		close(client_fd);  // parent doesn't need this
	}

	return 0;
}
