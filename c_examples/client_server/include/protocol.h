/*
 * This file contains a custom protocol for file transfer.
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define MESSAGE_NOT_SET 0
#define MESSAGE_ERROR 1
#define MESSAGE_REQUEST 2
#define MESSAGE_RESPONSE 3
#define MESSAGE_CHUNK 4

#define COMMAND_NOT_SET 0
#define COMMAND_REQUEST_FILE 1
#define COMMAND_REQUEST_METADATA 2
#define COMMAND_SEND_MESSAGE 3

#define STATUS_OK 0
#define STATUS_ERROR 1
#define STATUS_NOT_SET 100

#define ERROR_NOT_SET 0
#define ERROR_UNKNOWN_COMMAND 1
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_MAX_PAYLOAD_SIZE_EXCEEDED 3

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

#define HEADER_SIZE sizeof(Header)
#define MAX_PAYLOAD_SIZE 1024
#define MAX_MESSAGE_SIZE (HEADER_SIZE + MAX_PAYLOAD_SIZE)
#define HEADER_INIT {MESSAGE_NOT_SET, COMMAND_NOT_SET, 0, 0}

/**
 * @brief holds the header and payload (data) that will be sent over the network.
 * 
 * data: pointer to the byte array that we will send over the network, containing the header and payload
 * size: number of bytes (length) of the data we are sending
 */
typedef struct {
    uint8_t* data;
    uint32_t size;
} Message;


/**
 * @brief holds the parsed response header and payload data.
 * 
 * header: the parsed header metadata
 * payload: pointer to the parsed payload data
 * status: status of the response
 * error_code: error code if status is ERROR
 */
typedef struct {
    Header header;
    uint8_t* payload; 
    uint8_t status;
    uint8_t error_code;
} Response;

#define RESPONSE_INIT {HEADER_INIT, NULL, STATUS_NOT_SET, ERROR_NOT_SET}

/**
 * @brief Frees the memory allocated for the Message struct and resets the members to their default values.
 */
void destroy_message(Message* message);

/**
 * @brief Frees the memory allocated for the Response struct and resets the members to RESPONSE_INIT values.
 */
void destroy_response(Response* response);

/**
 * @brief Fills a Message struct with the byte array representation of a Header and payload that can be sent over the network.
 * 
 * Memory is allocated for the byte array in the Message struct.
 * The caller is responsible for freeing the memory, which can be done with the `destroy_message` function.
 * 
 * @param header Pointer to the Header struct.
 * @param payload Pointer to the byte array payload data.
 * @param message Pointer to a Message struct to store the final byte array and its size. The caller is responsible for freeing the memory via `destroy_message`.
 */
void create_message(const Header* header, const uint8_t* payload, Message* message);

/**
 * @brief Parses a raw byte array into a Response struct containing the Header and payload.
 * 
 * Memory is allocated for the payload in the Response struct.
 * The caller is responsible for freeing the memory, which can be done with the `destroy_response` function.
 * 
 * @param data Pointer to the byte array received over the network, containing the header and payload.
 * @param data_size The size of the byte array data.
 * @param response Pointer to a Response struct to store the parsed message. The caller is responsible for freeing the memory via `destroy_response`.
 */
void parse_message(const uint8_t* data, uint32_t data_size, Response* response);

#endif // PROTOCOL_H
