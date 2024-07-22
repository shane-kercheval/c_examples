#include "utils.h"
#include "sockets.h"
#include "protocol.h"
#include "file_transfer.h"
#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 9002
#define ADDRESS "0.0.0.0"

void* server_worker(void* arg) {
    int server_socket = bind_or_die(PORT);
    listen_or_die(server_socket, 1);
    int client_socket = accept_or_die(server_socket);

    uint8_t buffer[MAX_MESSAGE_SIZE];
    ssize_t bytes_received = receive_or_die(client_socket, buffer, MAX_MESSAGE_SIZE);
    Response response;
    int status = parse_message(buffer, bytes_received, &response);
    if (status != STATUS_OK) {
        fprintf(stderr, "Error parsing message\n");
        socket_cleanup(client_socket);
        socket_cleanup(server_socket);
        destroy_response(&response);
        return NULL;
    }
    // TODO: handle any request via handle_request(client_socket, &response.header, response.payload);
    send_file_metadata(client_socket, (const char*)response.payload);
    socket_cleanup(client_socket);
    socket_cleanup(server_socket);
    destroy_response(&response);
    return NULL;
}

void test__request_file_metadata__success() {
    pthread_t server_thread;
    int status = pthread_create(&server_thread, NULL, server_worker, NULL);
    if (status != 0) {
        perror("pthread_create");
        exit(1);
    }
    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    const char* file_name = "test.txt";
    Response response;
    status = request_file_metadata(server_socket, file_name, &response);
    pthread_join(server_thread, NULL);
    socket_cleanup(server_socket);

    char* expected_metadata = "Size: 35";
    uint32_t expected_payload_size = strlen_null_term(expected_metadata);

    TEST_ASSERT_EQUAL_INT(STATUS_OK, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_METADATA, response.header.command);
    TEST_ASSERT_EQUAL_UINT32(expected_payload_size, response.header.payload_size);
    TEST_ASSERT_EQUAL_UINT32(0, response.header.chunk_index);
    TEST_ASSERT_EQUAL_UINT8(STATUS_OK, response.header.status);
    TEST_ASSERT_EQUAL_UINT8(ERROR_NOT_SET, response.header.error_code);
    // check that the payload is correctly parsed/returned in the response
    TEST_ASSERT_TRUE(memcmp(response.payload, expected_metadata, expected_payload_size) == 0);
    destroy_response(&response);
}

void test__request_file_metadata__no_server_listening() {
    int socket = 0;
    const char* file_name = "test.txt";
    Response response;
    int status = request_file_metadata(socket, file_name, &response);
    TEST_ASSERT_EQUAL_INT(ERROR_SEND_FAILED, status);
}

void test__send_file_metadata__file_not_exists() {
    int socket = 0;
    const char* file_name = "this_file_does_not_exist.txt";
    int status = send_file_metadata(socket, file_name);
    TEST_ASSERT_EQUAL_INT(ERROR_FILE_NOT_FOUND, status);
}

void test__send_file_metadata__no_client_listening() {
    int socket = 0;
    const char* file_name = "test.txt";
    int status = send_file_metadata(socket, file_name);
    TEST_ASSERT_EQUAL_INT(ERROR_SEND_FAILED, status);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test__request_file_metadata__no_server_listening);
    RUN_TEST(test__send_file_metadata__file_not_exists);
    RUN_TEST(test__send_file_metadata__no_client_listening);
    RUN_TEST(test__request_file_metadata__success);
    return UNITY_END();
}
