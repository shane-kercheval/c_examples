#include "unity.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

void test__header_size_matches_last_offset_plus_one() {
    TEST_ASSERT_EQUAL_INT(HEADER_SIZE, HEADER_OFFSET_STATUS + 1);
}

void test__create_parse_message() {
    uint8_t payload[] = {'f', 'o', 'o', 'b', 'a', 'r'};
    uint32_t expected_payload_size = sizeof(payload);
    Header header = {MESSAGE_REQUEST, COMMAND_REQUEST_FILE, expected_payload_size, 0, NOT_SET};
    Message message;

    int rvalue = create_message(&header, payload, &message);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, rvalue);
    TEST_ASSERT_EQUAL_INT(sizeof(Header) + expected_payload_size, message.size);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_REQUEST, message.data[HEADER_OFFSET_MESSAGE_TYPE]);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, message.data[HEADER_OFFSET_COMMAND]);
    TEST_ASSERT_EQUAL_UINT32(htonl(expected_payload_size), *(uint32_t *)(message.data + HEADER_OFFSET_PAYLOAD_SIZE));
    TEST_ASSERT_EQUAL_UINT32(htonl(0), *(uint32_t *)(message.data + HEADER_OFFSET_CHUNK_INDEX));
    TEST_ASSERT_EQUAL_UINT8(NOT_SET, message.data[HEADER_OFFSET_STATUS]);
    // check payload is correctly copied into the message
    TEST_ASSERT_TRUE(memcmp(message.data + HEADER_SIZE, payload, expected_payload_size) == 0);
    
    Response response = RESPONSE_INIT;
    rvalue = parse_message(message.data, message.size, &response);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, rvalue);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_REQUEST, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, response.header.command);
    TEST_ASSERT_EQUAL_UINT32(expected_payload_size, response.header.payload_size);
    TEST_ASSERT_EQUAL_UINT32(0, response.header.chunk_index);
    TEST_ASSERT_EQUAL_UINT8(NOT_SET, response.header.status);
    // check that the payload is correctly parsed/returned in the response
    TEST_ASSERT_TRUE(memcmp(response.payload, payload, expected_payload_size) == 0);

    destroy_message(&message);
    destroy_response(&response);
}

void test__create_parse_message__max_payload_size() {
    uint8_t payload[MAX_PAYLOAD_SIZE];
    memset(payload, 'a', MAX_PAYLOAD_SIZE);  // fill payload with 'a'
    Header header = {MESSAGE_REQUEST, COMMAND_REQUEST_FILE, MAX_PAYLOAD_SIZE, 0, NOT_SET};
    
    Message message;
    int rvalue = create_message(&header, payload, &message);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, rvalue);
    TEST_ASSERT_EQUAL_INT(sizeof(Header) + MAX_PAYLOAD_SIZE, message.size);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_REQUEST, message.data[HEADER_OFFSET_MESSAGE_TYPE]);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, message.data[HEADER_OFFSET_COMMAND]);
    TEST_ASSERT_EQUAL_UINT32(htonl(MAX_PAYLOAD_SIZE), *(uint32_t *)(message.data + HEADER_OFFSET_PAYLOAD_SIZE));
    TEST_ASSERT_EQUAL_UINT32(htonl(0), *(uint32_t *)(message.data + HEADER_OFFSET_CHUNK_INDEX));
    TEST_ASSERT_EQUAL_UINT8(NOT_SET, message.data[HEADER_OFFSET_STATUS]);
    // check payload is correctly copied
    TEST_ASSERT_TRUE(memcmp(message.data + HEADER_SIZE, payload, MAX_PAYLOAD_SIZE) == 0);

    Response response = RESPONSE_INIT;
    rvalue = parse_message(message.data, message.size, &response);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, rvalue);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_REQUEST, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, response.header.command);
    TEST_ASSERT_EQUAL_UINT32(MAX_PAYLOAD_SIZE, response.header.payload_size);
    TEST_ASSERT_EQUAL_UINT32(0, response.header.chunk_index);
    TEST_ASSERT_EQUAL_UINT8(NOT_SET, response.header.status);
    // check that the payload is correctly parsed/returned in the response
    TEST_ASSERT_TRUE(memcmp(response.payload, payload, MAX_PAYLOAD_SIZE) == 0);

    destroy_message(&message);
    destroy_response(&response);
}

void test__create_message__exceed_max_payload_size() {
    uint8_t payload[MAX_PAYLOAD_SIZE + 1];
    memset(payload, 'a', MAX_PAYLOAD_SIZE + 1); // Fill payload with 'a'
    Header header = {MESSAGE_REQUEST, COMMAND_REQUEST_FILE, MAX_PAYLOAD_SIZE + 1, 0};
    Message message;

    int rvalue = create_message(&header, payload, &message);
    TEST_ASSERT_EQUAL_INT(ERROR_MAX_PAYLOAD_SIZE_EXCEEDED, rvalue);
}

void test__parse_message__invalid_data_size() {
    // Not enough data to form a complete header
    uint8_t data[HEADER_SIZE - 1];
    memset(data, 0, HEADER_SIZE - 1);

    Response response = RESPONSE_INIT;
    int rvalue = parse_message(data, HEADER_SIZE - 1, &response);
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_DATA_SIZE, rvalue);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test__header_size_matches_last_offset_plus_one);
    RUN_TEST(test__create_parse_message);
    RUN_TEST(test__create_parse_message__max_payload_size);
    RUN_TEST(test__create_message__exceed_max_payload_size);
    RUN_TEST(test__parse_message__invalid_data_size);
    return UNITY_END();
}
