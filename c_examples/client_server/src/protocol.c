#include "protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>


int create_message(const Header* header, const uint8_t* payload, Message* message) {
    if (header->payload_size > MAX_PAYLOAD_SIZE) {
        return ERROR_MAX_PAYLOAD_SIZE_EXCEEDED;
    }
    message->size = HEADER_SIZE + header->payload_size;
    // define a byte array of the size of the message
    message->data = (uint8_t*)malloc(message->size);
    if (message->data == NULL) {
        return ERROR_MEMORY_ALLOCATION_FAILED;
    }
    // first byte is the message type
    message->data[0] = header->message_type;
    // second byte is the command
    message->data[1] = header->command;
    // next four bytes are the payload size
    // transform the integer to network byte order (big-endian)
    // we need to transform it because, unlike the other fields, the these are multi-byte integer fields
    // (uint32_t*) provides us with a way to access 4 bytes of memory as a single 32-bit integer
    // then we have to dereference in order to assign the value to the memory location
    *(uint32_t*)(message->data + 2) = htonl(header->payload_size);
    // next four bytes is the chunk index
    *(uint32_t*)(message->data + 6) = htonl(header->chunk_index);
    // we have used 10 bytes so far, so we copy the payload after that
    if (header->payload_size > 0 && payload != NULL) {
        memcpy(message->data + 10, payload, header->payload_size);
    }
    return STATUS_OK;
}

int parse_message(const uint8_t* data, uint32_t data_size, Response* response) {
    if (data_size < HEADER_SIZE) {
        return ERROR_INVALID_DATA_SIZE;
    }
    // first byte is the message type
    response->header.message_type = data[0];
    // second byte is the command
    response->header.command = data[1];
    // next four bytes are the payload size
    // deconvert the integer from network byte order (big-endian) to host byte order
    response->header.payload_size = ntohl(*(uint32_t*)(data + 2));
    response->header.chunk_index = ntohl(*(uint32_t*)(data + 6));
    if (response->header.payload_size > 0) {
        if (data_size < (HEADER_SIZE + response->header.payload_size)) {
            return ERROR_INVALID_DATA_SIZE;
        }
        // create a byte array of the size of the payload and then copy the payload into it (starting at the 11th byte)
        response->payload = (uint8_t*)malloc(response->header.payload_size);
        if (response->payload == NULL) {
            return ERROR_MEMORY_ALLOCATION_FAILED;
        }
        memcpy(response->payload, data + 10, response->header.payload_size);
    } else {
        response->payload = NULL;
    }
    return STATUS_OK;
}

void destroy_message(Message* message) {
    if (message != NULL) {
        if (message->data != NULL) {
            free(message->data);
            message->data = NULL;
        }
        message->size = 0;
    }
}

void destroy_response(Response *response) {
    if (response != NULL) {
        if(response->payload != NULL) {
            free(response->payload);
            response->payload = NULL;
        }
        response->header = (Header)HEADER_INIT;
        response->status = STATUS_NOT_SET;
        response->error_code = ERROR_NOT_SET;
    }
}