/*
 * This file contains a custom protocol for file transfer.
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define NOT_SET 255

#define MESSAGE_REQUEST 1
#define MESSAGE_RESPONSE 2
#define MESSAGE_RESPONSE_CHUNK 3
#define MESSAGE_RESPONSE_LAST_CHUNK  4

#define COMMAND_REQUEST_FILE 1
#define COMMAND_REQUEST_METADATA 2

#define STATUS_OK 0
#define STATUS_ERROR 1

#define ERROR_UNKNOWN_COMMAND 1
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_MAX_PAYLOAD_SIZE_EXCEEDED 3
#define ERROR_MEMORY_ALLOCATION_FAILED 4
#define ERROR_INVALID_DATA_SIZE 5
#define ERROR_SEND_FAILED 6
#define ERROR_RECEIVE_FAILED 7
#define ERROR_FILE_OPEN_FAILED 8
#define ERROR_INVALID_COMMAND 9

#define HEADER_OFFSET_MESSAGE_TYPE 0
#define HEADER_OFFSET_COMMAND 1
#define HEADER_OFFSET_PAYLOAD_SIZE 2
#define HEADER_OFFSET_CHUNK_INDEX 6
#define HEADER_OFFSET_STATUS 10
#define HEADER_OFFSET_ERROR_CODE 11

#define MAX_PAYLOAD_SIZE 1024

/**
 * @brief Header struct that contains the metadata for a message.
 * 
 * message_type: type of message (request, response, error, etc.)
 * command: command type (request file, send message, etc.)
 * payload_size: size of the payload data in bytes
 * chunk_index: index of the chunk (for chunked messages)
 * status: status of the response (not used for requests)
 * error_code: error code if status is ERROR (not used for requests)
 */
#pragma pack(push, 1) // this ensures that the struct is packed with 1 byte alignment (i.e. without padding); this is needed so offset calculations are correct when converting creating the byte array message
typedef struct {
    // using unsigned integers because we won't have negative values
    // using 8-bit integers because we only need 1 byte for certain values
    // using 32-bit integers to ensure we use 32 bits across all platforms
    // specifying 8/32 allows use to build a protocol that can be consistently created/parsed across platforms
    uint8_t message_type;
    uint8_t command;
    uint32_t payload_size;
    uint32_t chunk_index;
    uint8_t status;
    uint8_t error_code;
} Header;
#pragma pack(pop)

#define HEADER_SIZE sizeof(Header)
#define MAX_MESSAGE_SIZE (HEADER_SIZE + MAX_PAYLOAD_SIZE)
#define HEADER_INIT {NOT_SET, NOT_SET, 0, 0, NOT_SET, NOT_SET}

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
 */
typedef struct {
    Header header;
    uint8_t* payload;
} Response;

#define RESPONSE_INIT {HEADER_INIT, NULL}

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
 * 
 * @return `STATUS_OK` if the message was created successfully, otherwise an error code starting with `ERROR_`.
 */
int create_message(const Header* header, const uint8_t* payload, Message* message);


/**
 * @brief Extracts the Header struct from a raw byte array.
 */
int extract_header(const uint8_t* data, uint32_t data_size, Header* header);

/**
 * @brief Parses a raw byte array into a Response struct containing the Header and payload.
 * 
 * Memory is allocated for the payload in the Response struct.
 * The caller is responsible for freeing the memory, which can be done with the `destroy_response` function.
 * 
 * @param data Pointer to the byte array received over the network, containing the header and payload.
 * @param data_size The size of the byte array data.
 * @param response Pointer to a Response struct to store the parsed message. The caller is responsible for freeing the memory via `destroy_response`.
 * 
 * @return `STATUS_OK` if the message was parsed successfully, otherwise an error code starting with `ERROR_`.
 */
int parse_message(const uint8_t* data, uint32_t data_size, Response* response);

#endif // PROTOCOL_H
