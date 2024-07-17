#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include "utils.h"

/**
 * @struct HttpRequest
 * @brief Represents an HTTP request with a request ID and data.
 */
typedef struct {
    int request_id;
    char data[256];
} HttpRequest;

/**
 * @struct Buffer
 * @brief Represents a circular buffer for storing HttpRequest objects.
 * 
 * @var Buffer::buffer
 * Buffer to store HttpRequest objects.
 * @var Buffer::size
 * Size of the buffer.
 * @var Buffer::get_index
 * Index that consumer reads from.
 * @var Buffer::put_index
 * Index that producer writes to.
 * @var Buffer::count
 * Number of items in the buffer.
 */
typedef struct {
    HttpRequest *buffer;
    int size;
    int get_index;
    int put_index;
    int count;
} Buffer;

/**
 * @brief Creates a buffer of the given size.
 * 
 * @param size Number of HttpRequest objects the buffer can store.
 * @return Pointer to the created Buffer.
 */
Buffer *create_buffer(int size);

/**
 * @brief Destroys/frees the given buffer.
 * 
 * @param buffer Pointer to the Buffer to be destroyed.
 */
void destroy_buffer(Buffer *buffer);

/**
 * @brief Gets the next HttpRequest from the buffer.
 * 
 * @param buffer Pointer to the Buffer.
 * @return The next HttpRequest in the buffer. Returns an HttpRequest with request_id -1 if the buffer is empty.
 */
HttpRequest get(Buffer *buffer);

/**
 * @brief Puts the given HttpRequest into the buffer.
 * 
 * @param buffer Pointer to the Buffer.
 * @param value The HttpRequest to be added to the buffer.
 * @return True if the HttpRequest was successfully added, false if the buffer is full.
 */
bool put(Buffer *buffer, HttpRequest value);

#endif // BUFFER_H
