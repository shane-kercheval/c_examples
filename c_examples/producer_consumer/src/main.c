/*
* Some of this code has been inspired by and/or modified from `Operating Systems: Three Easy Pieces (p. 383).`
*/
#include <stdio.h>
#include <unistd.h>
// #include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "utils.h"
#include "buffer.h"

// NDEBUG (No Debug) is defined in Release mode and removes assertion statements
#ifdef NDEBUG
    bool NDEBUG_DEFINED = true;
#else
    bool NDEBUG_DEFINED = false;
#endif

int total_requests = 0;
const int MAX_REQUESTS = 100;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// OSTEP: "producer threads wait on the condition `not_full`, and signals `not_empty`"
// OSTEP: "consumer threads wait on the condition `not_empty`, and signals `not_full`"
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

/*
 * Worker thread function that produces requests.
 * Required format:  void *(*start_routine)(void *)
*/
void* producer(void* arg) {
    Buffer* buffer = (Buffer*)arg;
    // generate fixed number of requests
    while (total_requests < MAX_REQUESTS) {
        MUTEX_LOCK(&mutex);
        while (buffer->count == buffer->size) {
            // When the count is equal to the size of the buffer, the buffer is full.
            // We are going to wait until the not_full condition is signaled.
            // We pass in mutex which will be unlocked while waiting and re-locked when signaled.
            COND_WAIT(&not_full, &mutex);
        }
        HttpRequest request;
        request.request_id = total_requests;
        sprintf(request.data, "%d", total_requests);
        put(buffer, request);
        total_requests++;
        // Signal the not_empty condition to wake up any waiting consumers.
        COND_SIGNAL(&not_empty);
        MUTEX_UNLOCK(&mutex);
        printf("Producer producing request %d: %s\n", request.request_id, request.data);
    }
    return NULL;
}

/*
* Worker thread function that consumes requests.
* Required format:  void *(*start_routine)(void *)
*/
void* consumer(void* arg) {
    Buffer* buffer = (Buffer*)arg;
    while (buffer->count != 0 || total_requests < MAX_REQUESTS) {
        MUTEX_LOCK(&mutex);
        while (buffer->count == 0) {
            // When the count is 0, the buffer is empty.
            // We are going to wait until the not_empty condition is signaled.
            // We pass in mutex which will be unlocked while waiting and re-locked when signaled.
            COND_WAIT(&not_empty, &mutex);
        }
        HttpRequest request = get(buffer);
        // Signal the not_full condition to wake up any waiting producers.
        COND_SIGNAL(&not_full);
        MUTEX_UNLOCK(&mutex);
        // Simulate request processing
        printf("Consumer processing request %d: %s\n", request.request_id, request.data);
        // sleep(1);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    int num_args = argc - 1;
    if (num_args != 3) {
        printf("Usage: <num_producers> <num_consumers> <buffer_size>\n");
        return 1;
    }    
    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    int buffer_size = atoi(argv[3]);
    
    pthread_t* producer_threads = (pthread_t*)malloc_or_die(num_producers * sizeof(pthread_t));
    pthread_t* consumer_threads = (pthread_t*)malloc_or_die(num_consumers * sizeof(pthread_t));
    Buffer* buffer_ptr = create_buffer(buffer_size);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < num_producers; i++) {
        PTHREAD_CREATE(&producer_threads[i], producer, (void*)buffer_ptr);
    }
    for (int i = 0; i < num_consumers; i++) {
        PTHREAD_CREATE(&consumer_threads[i], consumer, (void*)buffer_ptr);
    }
    for (int i = 0; i < num_producers; i++) {
        PTHREAD_JOIN(producer_threads[i]);
    }
    for (int i = 0; i < num_consumers; i++) {
        PTHREAD_JOIN(consumer_threads[i]);
    }

    gettimeofday(&end, NULL);
    long elapsed = duration(start, end);
    printf("Elapsed time: %ld microseconds\n", elapsed);

    printf("num_producers: %d\n", num_producers);
    printf("num_consumers: %d\n", num_consumers);
    printf("buffer_size: %d\n", buffer_size);
    printf("\nRunning in %s\n\n", NDEBUG_DEFINED ? "Release (NDEBUG defined; e.g. assertion statements removed)" : "Debug (NDEBUG not defined; e.g. assertion statements included)");
    
    free(producer_threads);
    free(consumer_threads);
    destroy_buffer(buffer_ptr);
    MUTEX_DESTROY(&mutex);
    return 0;
}
