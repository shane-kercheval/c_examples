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

void test__request_file_metadata__file_not_exist() {
    const char* file_name = "this_file_does_not_exist.txt";

    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    Response response;
    int status = request_file_metadata(server_socket, file_name, &response);
    socket_cleanup(server_socket);
    // fprintf(stderr, "response.header.status: %u\n", response.header.status);
    // fprintf(stderr, "response.header.error_code: %u\n", response.header.error_code);
    TEST_ASSERT_EQUAL_INT(ERROR_FILE_NOT_FOUND, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_METADATA, response.header.command);
    TEST_ASSERT_EQUAL_UINT8(ERROR_FILE_NOT_FOUND, response.header.status);
    destroy_response(&response);
}

void test__request_file_metadata__file_name_too_long() {
    char file_name[501]; memset(file_name, 'a', 500); file_name[500] = '\0';

    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    Response response;
    int status = request_file_metadata(server_socket, file_name, &response);
    socket_cleanup(server_socket);
    // fprintf(stderr, "response.header.status: %u\n", response.header.status);
    // fprintf(stderr, "response.header.error_code: %u\n", response.header.error_code);
    TEST_ASSERT_EQUAL_INT(ERROR_FILE_OPEN_FAILED, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_METADATA, response.header.command);
    TEST_ASSERT_EQUAL_UINT8(ERROR_FILE_OPEN_FAILED, response.header.status);
    destroy_response(&response);
}

void test__request_file_metadata__send_file_metadata__success() {
    const char* file_name = "test.txt";
    char* expected_metadata = "Size: 35";
    uint32_t expected_payload_size = strlen_null_term(expected_metadata);

    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    Response response;
    int status = request_file_metadata(server_socket, file_name, &response);
    socket_cleanup(server_socket);

    TEST_ASSERT_EQUAL_INT(STATUS_OK, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_METADATA, response.header.command);
    TEST_ASSERT_EQUAL_UINT32(expected_payload_size, response.header.payload_size);
    TEST_ASSERT_EQUAL_UINT32(0, response.header.chunk_index);
    TEST_ASSERT_EQUAL_UINT8(STATUS_OK, response.header.status);
    // check that the payload is correctly parsed/returned in the response
    TEST_ASSERT_TRUE(memcmp(response.payload, expected_metadata, expected_payload_size) == 0);
    destroy_response(&response);
}

void test__send_file_contents__file_not_exists() {
    int socket = 0;
    const char* file_name = "this_file_does_not_exist.txt";
    int status = send_file_contents(socket, file_name);
    TEST_ASSERT_EQUAL_INT(ERROR_FILE_NOT_FOUND, status);
}

void test__send_file_contents__file_name_too_long() {
    int socket = 0;
    char file_name[501]; memset(file_name, 'a', 500); file_name[500] = '\0';
    int status = send_file_contents(socket, file_name);
    TEST_ASSERT_EQUAL_INT(ERROR_FILE_OPEN_FAILED, status);
}

void test__send_file_contents__no_client_listening() {
    int socket = 0;
    const char* file_name = "test.txt";
    int status = send_file_contents(socket, file_name);
    TEST_ASSERT_EQUAL_INT(ERROR_SEND_FAILED, status);
}

void test__request_file_contents__file_not_exist() {
    const char* file_name = "file-does-not-exist";
   
    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    Response response;
    int status = request_file_contents(server_socket, file_name, &response);
    socket_cleanup(server_socket);
    // fprintf(stderr, "response.header.status: %u\n", response.header.status);
    // fprintf(stderr, "response.header.error_code: %u\n", response.header.error_code);
    TEST_ASSERT_EQUAL_INT(ERROR_FILE_NOT_FOUND, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, response.header.command);
    TEST_ASSERT_EQUAL_UINT8(ERROR_FILE_NOT_FOUND, response.header.status);
    destroy_response(&response);
}

void test__request_file_contents__file_name_too_long() {
    char file_name[501]; memset(file_name, 'a', 500); file_name[500] = '\0';
   
    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    Response response;
    int status = request_file_contents(server_socket, file_name, &response);
    socket_cleanup(server_socket);
    // fprintf(stderr, "response.header.status: %u\n", response.header.status);
    // fprintf(stderr, "response.header.error_code: %u\n", response.header.error_code);
    TEST_ASSERT_EQUAL_INT(ERROR_FILE_OPEN_FAILED, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, response.header.command);
    TEST_ASSERT_EQUAL_UINT8(ERROR_FILE_OPEN_FAILED, response.header.status);
    destroy_response(&response);
}

void test__request_file_contents__send_file_contents__success() {
    const char* file_name = "test.txt";
    const char* expected_contents = "These are the contents of test.txt\n";
    uint32_t expected_payload_size = strlen(expected_contents);

    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    Response response;
    int status = request_file_contents(server_socket, file_name, &response);
    socket_cleanup(server_socket);

    TEST_ASSERT_EQUAL_INT(STATUS_OK, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, response.header.command);
    TEST_ASSERT_EQUAL_UINT32(expected_payload_size, response.header.payload_size);
    TEST_ASSERT_EQUAL_UINT32(1, response.header.chunk_index + 1);
    TEST_ASSERT_EQUAL_UINT8(STATUS_OK, response.header.status);
    // check that the payload is correctly parsed/returned in the response
    TEST_ASSERT_TRUE(memcmp(response.payload, expected_contents, expected_payload_size) == 0);
    destroy_response(&response);
}

void test__request_file_contents__send_file_contents__multiple_chunks_success() {
    char full_path[256];
    const char* file_name = "test_multiple_chunks.txt";
    snprintf(full_path, sizeof(full_path), "%s/%s", SERVER_FILE_PATH, file_name);

    FILE* file = fopen(full_path, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }
    // read in the contents of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* expected_contents = (char*)malloc(file_size + 1);
    if (expected_contents == NULL) {
        perror("malloc");
        exit(1);
    }
    size_t bytes_read = fread(expected_contents, 1, file_size, file);
    fclose(file);
    // fread does not null terminate the string
    expected_contents[file_size] = '\0';
    if (bytes_read != file_size) {
        perror("fread");
        exit(1);
    }

    int expected_chunks = calculate_total_chunks(file_size);
    int server_socket = connect_with_retry_or_die(ADDRESS, PORT, 3, 1);
    Response response;
    int status = request_file_contents(server_socket, file_name, &response);
    socket_cleanup(server_socket);

    TEST_ASSERT_EQUAL_INT(STATUS_OK, status);
    TEST_ASSERT_EQUAL_UINT8(MESSAGE_RESPONSE, response.header.message_type);
    TEST_ASSERT_EQUAL_UINT8(COMMAND_REQUEST_FILE, response.header.command);
    TEST_ASSERT_EQUAL_UINT32(file_size, response.header.payload_size);
    TEST_ASSERT_EQUAL_UINT32(expected_chunks, response.header.chunk_index + 1);
    TEST_ASSERT_EQUAL_UINT8(STATUS_OK, response.header.status);
    // check that the payload is correctly parsed/returned in the response
    TEST_ASSERT_TRUE(memcmp(response.payload, expected_contents, file_size) == 0);
    destroy_response(&response);
    free(expected_contents);
}

void test__calculate_total_chunks() {
    TEST_ASSERT_EQUAL_INT(1, calculate_total_chunks(1));
    TEST_ASSERT_EQUAL_INT(1, calculate_total_chunks(MAX_PAYLOAD_SIZE - 1));
    TEST_ASSERT_EQUAL_INT(1, calculate_total_chunks(MAX_PAYLOAD_SIZE));
    TEST_ASSERT_EQUAL_INT(2, calculate_total_chunks(MAX_PAYLOAD_SIZE + 1));
    TEST_ASSERT_EQUAL_INT(2, calculate_total_chunks(MAX_PAYLOAD_SIZE * 2));
    TEST_ASSERT_EQUAL_INT(3, calculate_total_chunks((MAX_PAYLOAD_SIZE * 2) + 1));
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
    RUN_TEST(test__request_file_metadata__file_not_exist);
    RUN_TEST(test__request_file_metadata__file_name_too_long);
    RUN_TEST(test__request_file_metadata__send_file_metadata__success);
    RUN_TEST(test__send_file_contents__file_not_exists);
    RUN_TEST(test__send_file_contents__file_name_too_long);
    RUN_TEST(test__send_file_contents__no_client_listening);
    RUN_TEST(test__request_file_contents__file_not_exist);
    RUN_TEST(test__request_file_contents__send_file_contents__success);
    RUN_TEST(test__request_file_contents__send_file_contents__multiple_chunks_success);
    RUN_TEST(test__calculate_total_chunks);
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
