#include "protocol.h"
#include "file_transfer.h"
#include "unity.h"

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
    return UNITY_END();
}
