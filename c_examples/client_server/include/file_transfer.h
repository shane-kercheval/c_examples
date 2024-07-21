#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include "protocol.h"

/**
 * @brief Send a COMMAND_REQUEST_METADATA request to the server.
 * 
 * This method allocates memory for the response payload. The caller is responsible for freeing the memory via `destroy_response`.
 * 
 * @param socket the socket file descriptor of the server
 * @param file_name the name of the file to request metadata for
 * @param A pointer to a Response struct that will be filled containing the header and payload (metadata). The caller is responsible for freeing the memory via `destroy_response`.
 * 
 * @return 0 (STATUS_OK) if the request was successful, otherwise an error code starting with `ERROR_`.
 */
int request_file_metadata(int socket, const char* file_name, Response* response);


/**
 * @brief Handle a COMMAND_REQUEST_METADATA request from the client.
 * 
 * @param socket the socket file descriptor of the client
 * @param file_name the name of the file to send metadata for
 * 
 * @return 0 (STATUS_OK) if the request was successful, otherwise an error code starting with `ERROR_`.
 */
int send_file_metadata(int socket, const char* file_name);


/**
 * @brief Send a COMMAND_REQUEST_CONTENTS request to the server.
 * 
 * This method allocates memory for the response payload. The caller is responsible for freeing the memory via `destroy_response`.
 * 
 * @param socket the socket file descriptor of the server
 * @param file_name the name of the file to request metadata for
 * @param A pointer to a Response struct that will be filled containing the header and payload (metadata). The caller is responsible for freeing the memory via `destroy_response`.
 * 
 * @return 0 (STATUS_OK) if the request was successful, otherwise an error code starting with `ERROR_`.
 */
int request_file_contents(int socket, const char* file_name, Response* response);

/**
 * @brief Handle a COMMAND_REQUEST_CONTENTS request from the client.
 * 
 * @param socket the socket file descriptor of the client
 * @param file_name the name of the file to send metadata for 
 * 
 * @return 0 (STATUS_OK) if the request was successful, otherwise an error code starting with `ERROR_`.
 */
int send_file_contents(int socket, const char* file_name);


/**
 * @brief Handle a request from the client (e.g COMMAND_REQUEST_METADATA or COMMAND_REQUEST_CONTENTS).
 * 
 * @param socket the socket file descriptor of the client
 * @param header the header of the request
 * @param payload the payload of the request
 * 
 * @return 0 (STATUS_OK) if the request was successful, otherwise an error code starting with `ERROR_`.
 */
int handle_request(int socket, const Header* header, const uint8_t* payload);

#endif // FILE_TRANSFER_H
