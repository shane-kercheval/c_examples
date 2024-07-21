#include "protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>


void create_message(const Header* header, const uint8_t* payload, Message* message) {
    // TODO: what is the max payload size? What happens if it is larger i.e. chunk index out of range?
    message->size = sizeof(Header) + header->payload_size;
    // define a byte array of the size of the message
    message->data = (uint8_t*)malloc(message->size);
    if (message->data == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for message->data\n");
        // TODO: not sure if this is the best way to handle this error
        exit(EXIT_FAILURE);
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
}

void parse_message(const uint8_t* data, uint32_t data_size, Response* response) {
    if (data_size < sizeof(Header)) {
        fprintf(stderr, "Error: Data size is too small to contain the header\n");
        // TODO: not sure if this is the best way to handle this error
        exit(EXIT_FAILURE);
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
        if (data_size < (sizeof(Header) + response->header.payload_size)) {
            // TODO: what does it mean and what should we do if the data isn't large enough to contain the payload?
            exit(EXIT_FAILURE);
        }
        // create a byte array of the size of the payload and then copy the payload into it (starting at the 11th byte)
        response->payload = (uint8_t*)malloc(response->header.payload_size);
        if (response->payload == NULL) {
            // TODO: not sure if this is the best way to handle this error
            fprintf(stderr, "Error: Memory allocation failed for response.payload\n");
            exit(EXIT_FAILURE);
        }
        memcpy(response->payload, data + 10, response->header.payload_size);
    } else {
        response->payload = NULL;
    }
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
