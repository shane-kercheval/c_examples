// https://www.youtube.com/watch?v=mStnzIEprH8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

# define DEFAULT_PROTOCOL 0
# define PORT 3490
# define BACKLOG 10 
# define RESPONSE_SIZE 1024
# define HEADER_SIZE 2048


int main() {
    // we want to send an HTML response
    // we are essentially sending an html file
    // So we are going to use the c methods for opening and reading files
    FILE *html_file = fopen("./socket_tutorial/index.html", "r");
    if (html_file == NULL) {
        perror("fopen");
        exit(1);
    }
    // read the contents of the file
    // fgets only reads a single line
    // fgets(html_data, RESPONSE_SIZE, html_file);
    // Determine the file size
    fseek(html_file, 0, SEEK_END);
    long file_size = ftell(html_file);
    if (file_size > RESPONSE_SIZE - 1) {
        fprintf(stderr, "File size exceeds buffer limit\n");
        fclose(html_file);
        exit(1);
    }
    rewind(html_file); // Equivalent to fseek(html_file, 0, SEEK_SET);
    // Read the entire file into html_data
    char html_data[RESPONSE_SIZE] = {0}; // Initialize buffer to zero
    if (fread(html_data, 1, file_size, html_file) < file_size) {
        if (!feof(html_file)) { // Check if the read was incomplete due to an error
            perror("fread");
            fclose(html_file);
            exit(1);
        }
    }
    fclose(html_file); // Close the file as soon as we're done with it

    char html_response[HEADER_SIZE] = "HTTP/1.1 200 OK\r\n\n";
    // concatenate the header and the response data
    strcat(html_response, html_data);  // NOLINT
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

    // This is a socket option to allow the socket to be reused
    int yes = 1; // Enable the option
    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        close(server_socket_fd); // Ensure the socket is properly closed on error
        exit(1);
    }

    // bind the socket to our specified IP and port
    if (bind(server_socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("bind");
        exit(1);
    }
    // listen for connections
    listen(server_socket_fd, BACKLOG);
    printf("Server is listening on port %d\n", PORT);
    // accept a connection
    int client_socket_fd = -1;
    
    while(1) {
        client_socket_fd = accept(server_socket_fd, NULL, NULL);
        printf("Client connected\n");
        if (client_socket_fd == -1) {
            perror("accept");
            exit(1);
        }
        // send the message
        // 0 is a flag that indicates no special flags are set
        send(client_socket_fd, html_response, strlen(html_response), 0);
        close(client_socket_fd);
    }
    // close the socket
    close(server_socket_fd);
    return 0;
}
