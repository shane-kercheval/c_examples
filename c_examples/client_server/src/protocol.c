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
    message->data[HEADER_OFFSET_MESSAGE_TYPE] = header->message_type;
    // second byte is the command
    message->data[HEADER_OFFSET_COMMAND] = header->command;
    // next four bytes are the payload size
    // transform the integer to network byte order (big-endian)
    // we need to transform it because, unlike the other fields, the these are multi-byte integer fields
    // (uint32_t*) provides us with a way to access 4 bytes of memory as a single 32-bit integer
    // then we have to dereference in order to assign the value to the memory location
    *(uint32_t*)(message->data + HEADER_OFFSET_PAYLOAD_SIZE) = htonl(header->payload_size);
    // next four bytes is the chunk index
    *(uint32_t*)(message->data + HEADER_OFFSET_CHUNK_INDEX) = htonl(header->chunk_index);
    message->data[HEADER_OFFSET_STATUS] = header->status;
    if (header->payload_size > 0 && payload != NULL) {
        // copy the payload into the message starting after the header
        memcpy(message->data + HEADER_SIZE, payload, header->payload_size);
    }
    return STATUS_OK;
}

int extract_header(const uint8_t* data, uint32_t data_size, Header* header) {
    if (data_size < HEADER_SIZE) {
        return ERROR_INVALID_DATA_SIZE;
    }
    header->message_type = data[HEADER_OFFSET_MESSAGE_TYPE];
    header->command = data[HEADER_OFFSET_COMMAND];
    header->payload_size = ntohl(*(uint32_t*)(data + HEADER_OFFSET_PAYLOAD_SIZE));
    header->chunk_index = ntohl(*(uint32_t*)(data + HEADER_OFFSET_CHUNK_INDEX));
    header->status = data[HEADER_OFFSET_STATUS];
    return STATUS_OK;
}

int parse_message(const uint8_t* data, uint32_t data_size, Response* response) {
    Header header;
    int status = extract_header(data, data_size, &header);
    if (status != STATUS_OK) {
        return status;
    }
    response->header = header;
    if (response->header.payload_size > 0) {
        if (data_size < (HEADER_SIZE + response->header.payload_size)) {
            return ERROR_INVALID_DATA_SIZE;
        }
        // create a byte array of the size of the payload and then copy the payload into it (starting at the 11th byte)
        response->payload = (uint8_t*)malloc(response->header.payload_size);
        if (response->payload == NULL) {
            return ERROR_MEMORY_ALLOCATION_FAILED;
        }
        memcpy(response->payload, data + HEADER_SIZE, response->header.payload_size);
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
    }
}
