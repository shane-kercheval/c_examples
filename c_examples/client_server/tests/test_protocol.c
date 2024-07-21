#include "unity.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

void test_encode_decode_message() {
    uint8_t payload[] = {'f', 'o', 'o', 'b', 'a', 'r'};
    uint32_t expected_payload_size = sizeof(payload);
    Header header = {MESSAGE_REQUEST, COMMAND_REQUEST_FILE, expected_payload_size, 0};
    Message message;

    int result = create_message(&header, payload, &message);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, result);
    TEST_ASSERT_TRUE(message.size == sizeof(Header) + expected_payload_size);
    TEST_ASSERT_TRUE(message.data[0] == MESSAGE_REQUEST);
    TEST_ASSERT_TRUE(message.data[1] == COMMAND_REQUEST_FILE);
    TEST_ASSERT_EQUAL_UINT32(*(uint32_t *)(message.data + 2), htonl(expected_payload_size));
    TEST_ASSERT_EQUAL_UINT32(*(uint32_t *)(message.data + 6), htonl(0));
    // check payload is correctly copied
    TEST_ASSERT_TRUE(memcmp(message.data + 10, payload, expected_payload_size) == 0);
    
    Response response = RESPONSE_INIT;
    result = parse_message(message.data, message.size, &response);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, result);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_REQUEST, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, response.header.command);
    TEST_ASSERT_EQUAL_UINT32(expected_payload_size, response.header.payload_size);
    TEST_ASSERT_EQUAL_UINT32(0, response.header.chunk_index);
    TEST_ASSERT_TRUE(memcmp(response.payload, payload, expected_payload_size) == 0);

    destroy_message(&message);
    destroy_response(&response);
}

void test_max_payload_size() {
    uint8_t payload[MAX_PAYLOAD_SIZE];
    memset(payload, 'a', MAX_PAYLOAD_SIZE);  // fill payload with 'a'
    Header header = {MESSAGE_REQUEST, COMMAND_REQUEST_FILE, MAX_PAYLOAD_SIZE, 0};
    
    Message message;
    int result = create_message(&header, payload, &message);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, result);
    TEST_ASSERT_TRUE(message.size == sizeof(Header) + MAX_PAYLOAD_SIZE);
    // check payload is correctly copied
    TEST_ASSERT_TRUE(memcmp(message.data + 10, payload, MAX_PAYLOAD_SIZE) == 0);

    Response response = RESPONSE_INIT;
    result = parse_message(message.data, message.size, &response);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, result);
    TEST_ASSERT_EQUAL_UINT32(MAX_PAYLOAD_SIZE, response.header.payload_size);
    // check that the payload is correctly parsed/returned in the response
    TEST_ASSERT_TRUE(memcmp(response.payload, payload, MAX_PAYLOAD_SIZE) == 0);

    destroy_message(&message);
    destroy_response(&response);
}

void test_exceed_max_payload_size() {
    uint8_t payload[MAX_PAYLOAD_SIZE + 1];
    memset(payload, 'a', MAX_PAYLOAD_SIZE + 1); // Fill payload with 'a'
    Header header = {MESSAGE_REQUEST, COMMAND_REQUEST_FILE, MAX_PAYLOAD_SIZE + 1, 0};
    Message message;

    int result = create_message(&header, payload, &message);
    TEST_ASSERT_EQUAL_INT(ERROR_MAX_PAYLOAD_SIZE_EXCEEDED, result);
}

void test_invalid_data_size() {
    // Not enough data to form a complete header
    uint8_t data[HEADER_SIZE - 1];
    memset(data, 0, HEADER_SIZE - 1);

    Response response = RESPONSE_INIT;
    int result = parse_message(data, HEADER_SIZE - 1, &response);
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_DATA_SIZE, result);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_encode_decode_message);
    RUN_TEST(test_max_payload_size);
    RUN_TEST(test_exceed_max_payload_size);
    RUN_TEST(test_invalid_data_size);
    return UNITY_END();
}
