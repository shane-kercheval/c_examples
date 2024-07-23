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

int server_running = 1;
pthread_t server_thread;
pthread_mutex_t server_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * This function is a worker thread that acts as a server.
 */
void* server_worker(void* arg) {
    int server_socket = bind_or_die(PORT);
    listen_or_die(server_socket, 1);

    while (1) {
        int client_socket = accept_or_die(server_socket);
        // after accept, check if the server is still running
        pthread_mutex_lock(&server_mutex);
        if (!server_running) {
            pthread_mutex_unlock(&server_mutex);
            break;
        }
        pthread_mutex_unlock(&server_mutex);
        uint8_t buffer[MAX_MESSAGE_SIZE];
        ssize_t bytes_received = receive_or_die(client_socket, buffer, MAX_MESSAGE_SIZE);
        Response response;
        int status = parse_message(buffer, bytes_received, &response);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error parsing message\n");
            socket_cleanup(client_socket);
            continue;
        }
        handle_request(client_socket, &response.header, response.payload);
        socket_cleanup(client_socket);
        destroy_response(&response);
    }
    socket_cleanup(server_socket);
    return NULL;
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

void test__send_file_metadata__file_name_too_long() {
    int socket = 0;
    char file_name[501]; memset(file_name, 'a', 500); file_name[500] = '\0';
    int status = send_file_metadata(socket, file_name);
    TEST_ASSERT_EQUAL_INT(ERROR_FILE_OPEN_FAILED, status);
}

void test__send_file_metadata__no_client_listening() {
    int socket = 0;
    const char* file_name = "test.txt";
    int status = send_file_metadata(socket, file_name);
    TEST_ASSERT_EQUAL_INT(ERROR_SEND_FAILED, status);
}

void test__request_file_metadata__send_file_metadata__success() {
    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    const char* file_name = "test.txt";
    Response response;
    int status = request_file_metadata(server_socket, file_name, &response);
    socket_cleanup(server_socket);

    char* expected_metadata = "Size: 35";
    uint32_t expected_payload_size = strlen_null_term(expected_metadata);
    TEST_ASSERT_EQUAL_INT(STATUS_OK, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_METADATA, response.header.command);
    TEST_ASSERT_EQUAL_UINT32(expected_payload_size, response.header.payload_size);
    TEST_ASSERT_EQUAL_UINT32(0, response.header.chunk_index);
    TEST_ASSERT_EQUAL_UINT8(STATUS_OK, response.header.status);
    TEST_ASSERT_EQUAL_UINT8(NOT_SET, response.header.error_code);
    // check that the payload is correctly parsed/returned in the response
    TEST_ASSERT_TRUE(memcmp(response.payload, expected_metadata, expected_payload_size) == 0);
    destroy_response(&response);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    ////
    // start server in a separate thread
    ////
    int status = pthread_create(&server_thread, NULL, server_worker, NULL);
    if (status != 0) {
        perror("pthread_create");
        exit(1);
    }
    ////
    // run unit tests
    ////
    RUN_TEST(test__request_file_metadata__no_server_listening);
    RUN_TEST(test__send_file_metadata__file_not_exists);
    RUN_TEST(test__send_file_metadata__file_name_too_long);
    RUN_TEST(test__send_file_metadata__no_client_listening);
    RUN_TEST(test__request_file_metadata__send_file_metadata__success);
    ////
    // stop the server
    ////
    pthread_mutex_lock(&server_mutex);
    server_running = 0;
    pthread_mutex_unlock(&server_mutex);
    // send one more connection request to the server to interrupt the accept call
    int client_socket = connect_socket(ADDRESS, PORT);
    socket_cleanup(client_socket);
    // wait for the server to finish cleaning up
    pthread_join(server_thread, NULL);
    return UNITY_END();
}
