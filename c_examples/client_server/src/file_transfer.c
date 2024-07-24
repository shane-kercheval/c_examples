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

int _send_error_response(int socket, uint8_t command, uint8_t error_code, const char* error_message) {
    Header header;
    header.message_type = MESSAGE_RESPONSE;
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

int _send_request(int socket, uint8_t command, const char* file_name) {
    uint32_t payload_size = strlen_null_term(file_name);
    Header header;
    header.message_type = MESSAGE_REQUEST;
    header.command = command;
    header.payload_size = payload_size;
    header.chunk_index = 0;
    header.status = NOT_SET;
    header.error_code = NOT_SET;

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
    return STATUS_OK;
}

int request_file_metadata(int socket, const char* file_name, Response* response) {
    int status = _send_request(socket, COMMAND_REQUEST_METADATA, file_name);
    if (status != STATUS_OK) {
        return status;
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
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Error creating full path; status: %d", status);
        _send_error_response(socket, COMMAND_REQUEST_METADATA, ERROR_FILE_OPEN_FAILED, error_message);
        return ERROR_FILE_OPEN_FAILED;
    }
    // man page: The stat utility displays information about the file pointed to by file.
    struct stat file_stat;
    if (stat(full_path, &file_stat) == -1) {
        const char* error_message = "Error getting file stats";
        _send_error_response(socket, COMMAND_REQUEST_METADATA, ERROR_FILE_NOT_FOUND, error_message);
        return ERROR_FILE_NOT_FOUND;
    }

    char metadata[256];
    // let's just return the size of the file for now
    snprintf(metadata, sizeof(metadata), "Size: %ld", file_stat.st_size);
    // Header header = {MESSAGE_RESPONSE, COMMAND_REQUEST_METADATA, strlen_null_term(metadata), 0};
    Header header = {MESSAGE_RESPONSE, COMMAND_REQUEST_METADATA, strlen_null_term(metadata), 0, STATUS_OK, NOT_SET};
    Message message;
    status = create_message(&header, (const uint8_t*)metadata, &message);
    if (status != STATUS_OK) {
        destroy_message(&message); // free memory in case of error (we aren't sure if any was allocated)
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Error creating message; status: %d", status);
        _send_error_response(socket, COMMAND_REQUEST_METADATA, status, error_message);
        return status;
    }
    ssize_t bytes_sent = send(socket, message.data, message.size, 0);
    destroy_message(&message);
    if (bytes_sent <= 0) {
        const char* error_message = "Error sending message";
        _send_error_response(socket, COMMAND_REQUEST_METADATA, ERROR_SEND_FAILED, error_message);
        return ERROR_SEND_FAILED;
    }
    return STATUS_OK;
}

int request_file_contents(int socket, const char* file_name, Response* response) {
    uint32_t payload_size = strlen_null_term(file_name);
    Header header;
    header.message_type = MESSAGE_REQUEST;
    header.command = COMMAND_REQUEST_FILE;
    header.payload_size = payload_size;
    header.chunk_index = 0;
    header.status = NOT_SET;
    header.error_code = NOT_SET;

    Message message;
    int status = create_message(&header, (const uint8_t*)file_name, &message);
    if (status != STATUS_OK) {
        return status;
    }

    ssize_t bytes_sent = send(socket, message.data, message.size, 0);
    destroy_message(&message);
    if (bytes_sent <= 0) {
        return ERROR_SEND_FAILED;
    }

    uint8_t buffer[MAX_MESSAGE_SIZE];
    size_t total_bytes_received = 0;
    
    response->payload = NULL;
    while (1) {
        ssize_t bytes_received = recv(socket, buffer, MAX_MESSAGE_SIZE, 0);
        if (bytes_received <= 0) {
            status = ERROR_RECEIVE_FAILED;
            goto error;
        }

        Header temp_header;
        status = extract_header(buffer, bytes_received, &temp_header);
        if (status != STATUS_OK) {
            goto error;
        }
        
        // TODO: note that we aren't actually using/checking the chunk_index

        if (temp_header.message_type == MESSAGE_RESPONSE_CHUNK || temp_header.message_type == MESSAGE_RESPONSE_LAST_CHUNK) {
            // from man page:
            // void* realloc(void* ptr, size_t size);
            // The realloc() function tries to change the size of the allocation pointed to by ptr
            // to size, and returns ptr.  If there is not enough room to enlarge the memory allocation
            // pointed to by ptr, realloc() creates a new allocation, copies as much of the old
            // data pointed to by ptr as will fit to the new allocation, frees the old allocation,
            // and returns a pointer to the allocated memory.  If ptr is NULL, realloc() is identical
            // to a call to malloc() for size bytes.  If size is zero and ptr is not NULL, a new,
            // minimum sized object is allocated and the original object is freed.  When extending a region
            // allocated with calloc(3), realloc(3) does not guarantee that the additional memory is also zero-filled.
            // My understanding:
            // Always using the new pointer returned by realloc. It will either be the same as the
            // original pointer passed in (in which case it doesn't matter), or it will be a new pointer,
            // in which case the old pointer is freed and invalid.
            // TLDR; we are using realloc to continually grow the payload buffer as we receive more data
            uint8_t* new_payload = realloc(response->payload, total_bytes_received + temp_header.payload_size);
            if (new_payload == NULL) {
                status = ERROR_MEMORY_ALLOCATION_FAILED;
                goto error;
            }
            response->payload = new_payload;

            // from man page:
            //  void *memcpy(void *restrict dst, const void *restrict src, size_t n);
            // The memcpy() function copies n bytes from memory area src to memory area dst.
            // The first `total_bytes_received` bytes of response->payload will already be filled
            // with data from previous chunk. So we offset the pointer by `total_bytes_received`
            // to copy the new data from the buffer into the payload (start after the previous data).
            // We want to skip the header, so we start copying from `buffer + HEADER_SIZE`
            memcpy(response->payload + total_bytes_received, buffer + HEADER_SIZE, temp_header.payload_size);
            total_bytes_received += temp_header.payload_size;
            if (temp_header.message_type == MESSAGE_RESPONSE_LAST_CHUNK) {
                response->header = temp_header;
                response->header.message_type = MESSAGE_RESPONSE;
                response->header.payload_size = total_bytes_received;
                response->header.chunk_index = 0; // TODO: NOT SURE IF THIS IS EVEN USED
                response->header.status = STATUS_OK;
                response->header.error_code = NOT_SET;
                break;
            }
        }
        else if (temp_header.message_type == MESSAGE_RESPONSE && temp_header.status == STATUS_ERROR) {
            response->header = temp_header;
            status = response->header.error_code;
            goto error;
        }
        else {
            status = ERROR_UNEXPECTED_MESSAGE_TYPE;
            goto error;
        }
    }
    return STATUS_OK;

error:
    free(response->payload);
    response->payload = NULL;
    return status;
}

int send_file_contents(int socket, const char* file_name) {
    char full_path[256];
    int status = snprintf(full_path, sizeof(full_path), "%s/%s", SERVER_FILE_PATH, file_name);
    if (status < 0 || status >= sizeof(full_path)) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Error creating full path; status: %d", status);
        _send_error_response(socket, COMMAND_REQUEST_FILE, ERROR_FILE_OPEN_FAILED, error_message);
        return ERROR_FILE_OPEN_FAILED;
    }
    // rb is for reading in binary mode which is appropriate for transferring raw bytes
    FILE* file = fopen(full_path, "rb");
    if (file == NULL) {
        char error_message[500];
        snprintf(error_message, sizeof(error_message), "Error opening file: %s", full_path);
        return _send_error_response(socket, COMMAND_REQUEST_FILE, ERROR_FILE_NOT_FOUND, error_message);
    }
     // Get the total file size so that we can calculate the number of chunks (and set LAST_CHUNK)
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t buffer[MAX_PAYLOAD_SIZE];
    // we need to do this instead of checking if bytes_read < MAX_PAYLOAD_SIZE because the last chunk might be exactly MAX_PAYLOAD_SIZE
    uint32_t total_chunks = calculate_total_chunks(file_size);
    for (uint32_t chunk_index = 0; chunk_index < total_chunks; chunk_index++) {
        size_t bytes_read = fread(buffer, 1, MAX_PAYLOAD_SIZE, file);
        Header header;
        header.message_type = (chunk_index == total_chunks - 1) ? MESSAGE_RESPONSE_LAST_CHUNK : MESSAGE_RESPONSE_CHUNK;
        header.command = COMMAND_REQUEST_FILE;
        header.payload_size = bytes_read;
        header.chunk_index = chunk_index;
        header.status = STATUS_OK;
        header.error_code = NOT_SET;

        Message message;
        status = create_message(&header, buffer, &message);
        if (status != STATUS_OK) {
            destroy_message(&message);
            fclose(file);
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Error creating message (chunk %d), status: %d", chunk_index, status);
            return _send_error_response(socket, COMMAND_REQUEST_FILE, status, error_message);
        }
        ssize_t bytes_sent = send(socket, message.data, message.size, 0);
        destroy_message(&message);
        if (bytes_sent != message.size) {
            fclose(file);
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Error sending chunk %d, bytes_sent: %ld", chunk_index, bytes_sent);
            return _send_error_response(socket, COMMAND_REQUEST_FILE, ERROR_SEND_FAILED, error_message);
        }
    }
    fclose(file);
    return STATUS_OK;
}

int handle_request(int socket, const Header* header, const uint8_t* payload) {
    switch (header->command) {
        case COMMAND_REQUEST_METADATA:
            return send_file_metadata(socket, (const char*)payload);
        case COMMAND_REQUEST_FILE:
            return send_file_contents(socket, (const char*)payload);
        default:
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Invalid command: %d", header->command);
            return _send_error_response(socket, header->command, ERROR_INVALID_COMMAND, error_message);
    }
    return ERROR_INVALID_COMMAND;
}

int calculate_total_chunks(long file_size) {
    // e.g file_size = 1, MAX_PAYLOAD_SIZE = 1024, then `file_size + MAX_PAYLOAD_SIZE - 1` = 1024; 1024 / 1024 = 1
    // e.g file_size = 1024, MAX_PAYLOAD_SIZE = 1024, then `file_size + MAX_PAYLOAD_SIZE - 1` = 2047; 2047 / 1024 = 1
    // e.g file_size = 1025, MAX_PAYLOAD_SIZE = 1024, then `file_size + MAX_PAYLOAD_SIZE - 1` = 2048; 2048 / 1024 = 2
    return (file_size + MAX_PAYLOAD_SIZE - 1) / MAX_PAYLOAD_SIZE;
}
