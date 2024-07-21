#include "utils.h"
#include "protocol.h"
#include "file_transfer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>

#define SERVER_FILE_PATH "/code/c_examples/client_server/tests/fake_server_files"

int request_file_metadata(int socket, const char* file_name, Response* response) {
    uint32_t payload_size = strlen_null_term(file_name);
    Header header;
    header.message_type = MESSAGE_REQUEST;
    header.command = COMMAND_REQUEST_METADATA;
    header.payload_size = payload_size;
    header.chunk_index = 0;
    header.status = STATUS_NOT_SET;
    header.error_code = ERROR_NOT_SET;

    // convert string (filename) to raw bytes
    Message message;
    int status = create_message(&header, (const uint8_t*)file_name, &message);
    if (status != STATUS_OK) {
        return status;
    }
    // send the message (byte array) to the server
    // Man page: "Upon successful completion, the number of bytes which were sent is returned. 
    // Otherwise, -1 is returned..."
    ssize_t bytes_sent = send(socket, message.data, message.size, 0);
    destroy_message(&message); // free memory allocated in `create_message` before continuing/returning
    if (bytes_sent <= 0) {
        return ERROR_SEND_FAILED;
    }
    // receive the data from the server
    uint8_t buffer[MAX_MESSAGE_SIZE];
    ssize_t bytes_received = recv(socket, buffer, MAX_MESSAGE_SIZE, 0);
    if (bytes_received <= 0) {
        return ERROR_RECEIVE_FAILED;
    }
    status = parse_message(buffer, bytes_received, response);
    return status;
}

int _send_error_response(int socket, uint8_t command, uint8_t error_code) {
    char error_message[256];
    snprintf(error_message, sizeof(error_message), "Error code: %d", error_code);
    Header header;
    header.message_type = MESSAGE_ERROR;
    header.command = command;
    header.payload_size = strlen_null_term(error_message);
    header.chunk_index = 0;
    header.status = STATUS_ERROR;
    header.error_code = error_code;

    Message message;
    int status = create_message(&header, (const uint8_t*)error_message, &message);
    if (status != STATUS_OK) {
        destroy_message(&message); // free memory in case of error (we aren't sure if any was allocated)
        return status;
    }
    send(socket, message.data, message.size, 0);
    destroy_message(&message);
    return error_code;
}

int send_file_metadata(int socket, const char* file_name) {
    // the file name does not includfe the path, so we need to prepend the path
    char full_path[256];
    // Concatenate the full path with the file name
    // From man page:
    // The snprintf() function will write at most size-1 of the characters printed into
    // the output string (the size'th character then gets the terminating ‘\0’)
    // if the return value is greater than or equal to the size argument, the string was too short
    // and some of the printed characters were discarded. 
    int status = snprintf(full_path, sizeof(full_path), "%s/%s", SERVER_FILE_PATH, file_name);
    if (status < 0 || status >= sizeof(full_path)) {
        _send_error_response(socket, COMMAND_REQUEST_METADATA, ERROR_FILE_OPEN_FAILED);
        return ERROR_FILE_OPEN_FAILED;
    }
    // man page: The stat utility displays information about the file pointed to by file.
    struct stat file_stat;
    if (stat(full_path, &file_stat) == -1) {
        _send_error_response(socket, COMMAND_REQUEST_METADATA, ERROR_FILE_NOT_FOUND);
        return ERROR_FILE_NOT_FOUND;
    }

    char metadata[256];
    // let's just return the size of the file for now
    snprintf(metadata, sizeof(metadata), "Size: %ld", file_stat.st_size);
    Header header = {MESSAGE_RESPONSE, COMMAND_REQUEST_METADATA, strlen_null_term(metadata), 0};
    Message message;
    status = create_message(&header, (const uint8_t*)metadata, &message);
    if (status != STATUS_OK) {
        destroy_message(&message); // free memory in case of error (we aren't sure if any was allocated)
        _send_error_response(socket, COMMAND_REQUEST_METADATA, status);
        return status;
    }
    ssize_t bytes_sent = send(socket, message.data, message.size, 0);
    destroy_message(&message);
    if (bytes_sent <= 0) {
        _send_error_response(socket, COMMAND_REQUEST_METADATA, ERROR_SEND_FAILED);
        return ERROR_SEND_FAILED;
    }
    return STATUS_OK;
}

int request_file_contents(int socket, const char* file_name, Response* response) {
    return 0;
}

int send_file_contents(int socket, const char* file_name) {
    return 0;
}

int handle_request(int socket, const Header* header, const uint8_t* payload) {
    return 0;
}