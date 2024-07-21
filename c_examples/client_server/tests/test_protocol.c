#include "unity.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>


void test_encode_decode_message() {
    uint8_t payload[] = {'f', 'o', 'o', 'b', 'a', 'r'};
    uint32_t expected_payload_size = sizeof(payload);
    Header header = {MESSAGE_REQUEST, COMMAND_SEND_MESSAGE, expected_payload_size, 0};
    Message message;

    create_message(&header, payload, &message);
    TEST_ASSERT_TRUE(message.size == sizeof(Header) + expected_payload_size);
    TEST_ASSERT_TRUE(message.data[0] == MESSAGE_REQUEST);
    TEST_ASSERT_TRUE(message.data[1] == COMMAND_SEND_MESSAGE);
    TEST_ASSERT_EQUAL_UINT32(*(uint32_t *)(message.data + 2), htonl(expected_payload_size));
    TEST_ASSERT_EQUAL_UINT32(*(uint32_t *)(message.data + 6), htonl(0));
    TEST_ASSERT_TRUE(memcmp(message.data + 10, payload, expected_payload_size) == 0);
    
    Response response = RESPONSE_INIT;
    parse_message(message.data, message.size, &response);
    TEST_ASSERT_EQUAL_UINT8(response.header.message_type, MESSAGE_REQUEST);
    TEST_ASSERT_EQUAL_UINT8(response.header.command, COMMAND_SEND_MESSAGE);
    TEST_ASSERT_EQUAL_UINT32(response.header.payload_size, expected_payload_size);
    TEST_ASSERT_EQUAL_UINT32(response.header.chunk_index, 0);
    TEST_ASSERT_TRUE(memcmp(response.payload, payload, expected_payload_size) == 0);

    destroy_message(&message);
    destroy_response(&response);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_encode_decode_message);
    return UNITY_END();
}
