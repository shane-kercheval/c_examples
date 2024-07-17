#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "buffer.h"


Buffer *create_buffer(int size) {
    Buffer *buffer = (Buffer *)malloc_or_die(sizeof(Buffer));
    buffer->buffer = (HttpRequest *)malloc_or_die(size * sizeof(HttpRequest));
    buffer->size = size;
    buffer->get_index = 0;
    buffer->put_index = 0;
    buffer->count = 0;
    return buffer;
}

void destroy_buffer(Buffer *buffer) {
    free(buffer->buffer);
    free(buffer);
}

HttpRequest get(Buffer *buffer) {
    if (buffer->count == 0) {
        return (HttpRequest) { .request_id = -1, .data = "" };
    }
    HttpRequest value = buffer->buffer[buffer->get_index];
    // add to the index (so the next get grabs from the next index) and then loop to 0 if we reach the end
    buffer->get_index = (buffer->get_index + 1) % buffer->size;
    buffer->count--;
    return value;
}

bool put(Buffer *buffer, HttpRequest value) {
    if (buffer->count == buffer->size) {
        return false;
    }
    buffer->buffer[buffer->put_index] = value;
    // add to the index and then loop to 0 if we reach the end
    buffer->put_index = (buffer->put_index + 1) % buffer->size;
    buffer->count++;
    return true;
}
