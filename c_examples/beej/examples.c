#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    printf("Number of arguments: %d\n", argc - 1);
    char *protocal = "tcp";
    if (argc > 1) {
        protocal = argv[1];
    }
    printf("Protocol: %s\n", protocal);
    struct protoent *proto = getprotobyname(protocal);
    if (proto != NULL) {
        printf("Portocol name: %s\n", proto->p_name);
        printf("Portocol number: %d\n", proto->p_proto);

    } else {
        printf("Failed to get protocol by name.\n");
    }
}
