/*
 * This file contains a custom protocol for file transfer.
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define MESSAGE_ERROR 0
#define MESSAGE_REQUEST 1
#define MESSAGE_RESPONSE 2
#define MESSAGE_CHUNK 3

#define COMMAND_REQUEST_FILE 1
#define COMMAND_REQUEST_METADATA 2
#define COMMAND_SEND_MESSAGE 3

#define STATUS_OK 0
#define STATUS_ERROR 1

#define ERROR_UNKNOWN_COMMAND 1
#define ERROR_FILE_NOT_FOUND 2


/**
 * @brief Header struct that contains the metadata for a message.
 * 
 * message_type: type of message (request, response, error, etc.)
 * command: command type (request file, send message, etc.)
 * payload_size: size of the payload data in bytes
 * chunk_index: index of the chunk (for chunked messages)
 */
typedef struct {
    // using unsigned integers because we won't have negative values
    // using 8-bit integers because we only need 1 byte for certain values
    // using 32-bit integers to ensure we use 32 bits across all platforms
    // specifying 8/32 allows use to build a protocol that can be consistently created/parsed across platforms
    uint8_t message_type;
    uint8_t command;
    uint32_t payload_size;
    uint32_t chunk_index;
} Header;

/**
 * @brief holds the header and payload (data) that will be sent over the network.
 * 
 * data: pointer to the byte array that we will send over the network, containing the header and payload
 * size: number of bytes (length) of the data we are sending
 */
typedef struct {
    uint8_t *data;
    uint32_t size;
} Message;

/**
 * @brief holds the parsed response header and payload data.
 * 
 * header: the parsed header metadata
 * payload: pointer to the parsed payload data
 */
typedef struct {
    Header header;  // Header containing the response metadata
    uint8_t *payload;   // Pointer to the parsed payload
} Response;

/**
 * @brief Creates a Message (byte array) that can be sent over the network from a request Header and payload.
 * 
 * Memory is allocated for the byte array in the Message struct.
 * The caller is responsible for freeing the memory, which can be done with the `free_message` function.
 * 
 * @param header Pointer to the Header struct.
 * @param payload Pointer to the byte array payload data.
 * @return Message struct containing the final byte array and its size. The caller is responsible for freeing the memory via `free_message`.
 */
Message create_message(const Header *header, const uint8_t *payload);

/**
 * @brief Parses a raw byte array into a Response struct containing the Header and payload.
 * 
 * Memory is allocated for the payload in the Response struct.
 * The caller is responsible for freeing the memory, which can be done with the `free_response` function.
 * 
 * @param data Pointer to the byte array received over the network, containing the header and payload.
 * @param data_size The size of the byte array data.
 * @param response Pointer to a Response struct to store the decoded message. The caller is responsible for freeing the memory via `free_response`.
 */
void parse_message(const uint8_t *data, uint32_t data_size, Response *response);

/**
 * Frees the memory allocated for the created message.
 * 
 * @param message Pointer to the Message struct to free
 */
void free_message(Message *message);

/**
 * Frees the memory allocated for the parsed response message.
 * 
 * @param response Pointer to the Response struct to free
 */
void free_response(Response *response);

#endif // PROTOCOL_H
