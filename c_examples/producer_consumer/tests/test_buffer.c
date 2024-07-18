#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "buffer.h"

// https://stackoverflow.com/questions/1067226/c-multi-line-macro-do-while0-vs-scope-block
#define assert_true(condition) do { \
    if (!(condition)) { \
        fprintf( \
            stderr, \
            "Assertion failed: function %s, file %s, line %d.\n", \
            __func__, __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)


void test_create_destroy_buffer() {
    Buffer *buffer = create_buffer(10);
    assert_true(buffer != NULL);
    assert_true(buffer->size == 10);
    assert_true(buffer->count == 0);
    assert_true(buffer->total_puts == 0);
    assert_true(buffer->get_index == 0);
    assert_true(buffer->put_index == 0);
    destroy_buffer(buffer);
}

void test_put_get() {
    Buffer *buffer = create_buffer(10);
    HttpRequest req = { .request_id = 1, .data = "Test Request" };
    assert_true(put(buffer, req) == true);
    assert_true(buffer->count == 1);
    assert_true(buffer->total_puts == 1);
    assert_true(buffer->get_index == 0);
    assert_true(buffer->put_index == 1);
    assert_true(buffer->buffer[0].request_id == 1);
    assert_true(strcmp(buffer->buffer[0].data, "Test Request") == 0);

    HttpRequest retrieved = get(buffer);
    assert_true(retrieved.request_id == 1);
    assert_true(strcmp(retrieved.data, "Test Request") == 0);
    assert_true(buffer->count == 0);
    assert_true(buffer->total_puts == 1);
    assert_true(buffer->get_index == 1);
    assert_true(buffer->put_index == 1);

    destroy_buffer(buffer);
}

void test_empty_buffer() {
    Buffer *buffer = create_buffer(10);
    HttpRequest retrieved = get(buffer);
    assert_true(retrieved.request_id == -1);
    assert_true(strcmp(retrieved.data, "") == 0);
    destroy_buffer(buffer);
}

void test_full_buffer() {
    Buffer *buffer = create_buffer(2);
    HttpRequest req1 = { .request_id = 1, .data = "Request 1" };
    HttpRequest req2 = { .request_id = 2, .data = "Request 2" };
    HttpRequest req3 = { .request_id = 3, .data = "Request 3" };

    // fill the buffer
    assert_true(put(buffer, req1) == true);
    assert_true(put(buffer, req2) == true);
    assert_true(buffer->count == 2);
    assert_true(buffer->total_puts == 2);
    // buffer is full
    assert_true(put(buffer, req3) == false);
    assert_true(buffer->count == 2);
    assert_true(buffer->total_puts == 2);
    // try again
    assert_true(put(buffer, req3) == false);
    assert_true(buffer->count == 2);
    assert_true(buffer->total_puts == 2);

    // empty the buffer
    HttpRequest retrieved = get(buffer);
    assert_true(retrieved.request_id == 1);
    assert_true(strcmp(retrieved.data, "Request 1") == 0);
    assert_true(buffer->count == 1);
    retrieved = get(buffer);
    assert_true(retrieved.request_id == 2);
    assert_true(strcmp(retrieved.data, "Request 2") == 0);
    assert_true(buffer->count == 0);
    assert_true(buffer->total_puts == 2);

    // buffer is empty
    retrieved = get(buffer);
    assert_true(retrieved.request_id == -1);
    // try again
    retrieved = get(buffer);
    assert_true(retrieved.request_id == -1);
    assert_true(buffer->count == 0);

    destroy_buffer(buffer);
}

void test_circular_behavior() {
    Buffer *buffer = create_buffer(2);
    HttpRequest req1 = { .request_id = 1, .data = "Request 1" };
    HttpRequest req2 = { .request_id = 2, .data = "Request 2" };

    assert_true(put(buffer, req1) == true);
    assert_true(put(buffer, req2) == true);

    HttpRequest retrieved1 = get(buffer);
    assert_true(retrieved1.request_id == 1);
    assert_true(strcmp(retrieved1.data, "Request 1") == 0);

    HttpRequest retrieved2 = get(buffer);
    assert_true(retrieved2.request_id == 2);
    assert_true(strcmp(retrieved2.data, "Request 2") == 0);

    HttpRequest req3 = { .request_id = 3, .data = "Request 3" };
    assert_true(put(buffer, req3) == true);

    HttpRequest retrieved3 = get(buffer);
    assert_true(retrieved3.request_id == 3);
    assert_true(strcmp(retrieved3.data, "Request 3") == 0);

    destroy_buffer(buffer);
}

int main() {
    test_create_destroy_buffer();
    test_put_get();
    test_empty_buffer();
    test_full_buffer();
    test_circular_behavior();
    return 0;
}
