#include <stdio.h>
#include <unistd.h>
// #include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "utils.h"

// NDEBUG is defined in Release mode
#ifdef NDEBUG
    bool NDEBUG_DEFINED = true;
#else
    bool NDEBUG_DEFINED = false;
#endif


#define BUFFER_SIZE 10
#define MOVING_AVG_WINDOW 5

typedef struct {
    int request_id;
    char data[256];
} HttpRequest;

typedef struct {
    HttpRequest *buffer;
    int size;  // size of the buffer
    int get_index; // index that consumer reads from
    int put_index; // index that producer writes to
    int count;  // number of items in the buffer
} Buffer;

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


/*
 * Get the next HttpRequest from the buffer.
*/
HttpRequest get(Buffer buffer) {
    HttpRequest value = buffer.buffer[buffer.get_index];
    // add to the buffer index (so the next get grabs from the next index) and then loop to 0 if we reach the end
    buffer.get_index = (buffer.get_index + 1) % BUFFER_SIZE;
    buffer.count--;
    return value;
}


/*
 * Put the HttpRequest into the buffer.
*/
void put(Buffer buffer, HttpRequest value) {
    buffer.buffer[buffer.put_index] = value;
    // add to the buffer and then loop to 0 if we reach the end
    buffer.put_index = (buffer.put_index + 1) % BUFFER_SIZE;
}



int main(int argc, char *argv[]) {
    // printf("Num args: %d\n", argc);
    // for (int i = 0; i < argc; i++) {
    //     printf("Arg %d: %s\n", i, argv[i]);
    // }
    printf("\n\nRelease Mode (NDEBUG DEFINED): %s\n\n", NDEBUG_DEFINED ? "true" : "false");
    printf("hello\n");
    struct timeval start, end;
    gettimeofday(&start, NULL);
    sleep(1);
    gettimeofday(&end, NULL);
    long elapsed = duration(start, end);
    printf("Elapsed time: %ld microseconds\n", elapsed);
    printf("goodbye\n");

    return 0;
}
