/*
** showip.c -- show IP addresses for a host given on the command line
** Adapted from: https://beej.us/guide/bgnet/examples/showip.c
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(int argc, char *argv[]) {
    // `argc` is the number of arguments passed to the program (including the program name)
    // `argv` is an array of strings containing the arguments (including the program name)
    // argv[0] is the name of the program
    int num_arguments = argc - 1;
    char *site = argv[1];
    int status = 0;
    struct addrinfo hints, *results = NULL;
    char ip_string[INET6_ADDRSTRLEN];
    if (num_arguments != 1) {
        fprintf(stderr, "Expected 1 argument, but %d were provided.\n", num_arguments);
        return 1;
    }
    // addrinfo is "used to prep the socket address structures for subsequent use. It’s also used
    // in host name lookups, and service name lookups... You might not usually need to write to
    // these structures; oftentimes, a call to getaddrinfo() to fill out your struct addrinfo for
    // you is all you’ll need. You will, however, have to peer inside these structs to get the
    // values out" -- Beej
    // struct addrinfo {
    //     int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    //     int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    //     int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    //     int              ai_protocol;  // use 0 for "any"
    //     size_t           ai_addrlen;   // size of ai_addr in bytes
    //     struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    //     char            *ai_canonname; // full canonical hostname

    //     struct addrinfo *ai_next;      // linked list, next node
    // };
    // "hints argument points to an addrinfo structure that specifies criteria for selecting the
    // socket address structures returned in the list pointed to by res" (i.e results)
    // https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
    // make sure hints is emptied out
    // From clang-tidy linter: Call to function 'memset' is insecure as it does not provide
    // security checks introduced in the C11 standard. Replace with analogous functions that
    // support length arguments or provides boundary checks such as 'memset_s' in case of C11.
    memset(&hints, 0, sizeof(hints));  // NOLINT
    //AF_UNSPEC allow our code can be IP version-agnostic.
    hints.ai_family = AF_UNSPEC;  // AF_INET or AF_INET6 to force IPv4 vs IPv6
    // From ChatGPT:
    // If your showip.c program is primarily used for resolving and displaying IP addresses for a
    // given hostname (without establishing a connection to the host), changing the socket type to
    // SOCK_DGRAM might not produce a noticeable difference in its main functionality. This is
    // because the address resolution process itself does not depend on the socket type; it merely
    // influences the type of addresses getaddrinfo() returns as suitable for the specified socket
    // type.
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(site, NULL, &hints, &results)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }
    // we want to cycle through the results (linked list) of addresses e.g. IPv4 and IPv6
    for (struct addrinfo *p = results; p != NULL; p = p->ai_next) {
        void *ip_address = NULL;
        char *ip_version = NULL;
        // get the pointer to the address itself, which are different fields inside of different
        // structs
        // AF_INET stands for "Address Family, Internet" and specifies IPv4
        if (p->ai_family == AF_INET) {  // IPv4
            // sockaddr_in is a struct that contains an internet address for IPv4
            // convert sockaddr to sockaddr_in
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            ip_address = &(ipv4->sin_addr);
            ip_version = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            ip_address = &(ipv6->sin6_addr);
            ip_version = "IPv6";
        }
        // conver the IP to a s tring and print:
        // "Internet Network Address to Presentation"
        // “ntop” means “network to presentation”
        inet_ntop(p->ai_family, ip_address, ip_string, sizeof(ip_string));
        printf("  %s: %s\n", ip_version, ip_string);

        // socket() simply returns to you a socket descriptor that you can use in later system
        // calls, or -1 on error. The global variable errno is set to the error’s value 
        int socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        printf("Socket: %d\n", socket_fd);
    }
}
