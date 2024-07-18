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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// OSTEP: "producer threads wait on the condition `not_full`, and signals `not_empty`"
// OSTEP: "consumer threads wait on the condition `not_empty`, and signals `not_full`"
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

typedef struct {
    int thread_id;
    int max_requests;
    Buffer* buffer;
} ThreadArgs;

/*
 * Worker thread function that produces requests.
 * Required format:  void *(*start_routine)(void *)
*/
void* producer(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Buffer* buffer = args->buffer;
    int thread_id = args->thread_id;
    int max_requests = args->max_requests;

    // generate fixed number of requests
    // while (total_requests < max_requests) {   // this is causing deadlock because total_requests is not locked
    while (true) {
        MUTEX_LOCK(&mutex);
        if (total_requests >= max_requests) {
            // this needs to run before we put the request in the buffer and increment the total_requests
            // otherwise we will add something to the buffer and then exit the function before
            // signaling the not_empty condition and unlocking
            // I only have this break logic here to stop the program at a certain specified number of requests
            // Additionally, we have to broadcast when we are done because there might be multiple consumers,
            // and if we only called COND_SIGNAL, only one consumer would wake up and the others would be stuck.
            COND_BROADCAST(&not_empty);
            MUTEX_UNLOCK(&mutex);
            printf("Producer %d finished\n", thread_id);
            break;
        }
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
        // printf("Producer producing request %d: %s\n", request.request_id, request.data);
    }
    return NULL;
}

/*
* Worker thread function that consumes requests.
* Required format:  void *(*start_routine)(void *)
*/
void* consumer(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Buffer* buffer = args->buffer;
    int thread_id = args->thread_id;
    int max_requests = args->max_requests;

    while (true) {
        MUTEX_LOCK(&mutex);
        if (buffer->count == 0 && total_requests >= max_requests) {
            // no need to signal the producer threads because they are finished
            // I only have this break logic here to stop the program at a certain specified number of requests
            MUTEX_UNLOCK(&mutex);
            printf("Consumer %d finished\n", thread_id);
            break;
        }
        while (buffer->count == 0 && total_requests < max_requests) {
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
        // printf("Consumer processing request %d: %s\n", request.request_id, request.data);
        // sleep(1);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    int num_args = argc - 1;
    if (num_args != 4) {
        printf("Usage: <num_producers> <num_consumers> <buffer_size> <max_requests>\n");
        return 1;
    }    
    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    int buffer_size = atoi(argv[3]);
    int max_requests = atoi(argv[4]);
    
    pthread_t* producer_threads = (pthread_t*)malloc_or_die(num_producers * sizeof(pthread_t));
    pthread_t* consumer_threads = (pthread_t*)malloc_or_die(num_consumers * sizeof(pthread_t));
    Buffer* buffer_ptr = create_buffer(buffer_size);

    printf("Running producer-consumer simulation with %d producers, %d consumers, buffer size %d, max requests %d\n", num_producers, num_consumers, buffer_size, max_requests);
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < num_producers; i++) {
        ThreadArgs* args = (ThreadArgs*)malloc_or_die(sizeof(ThreadArgs));
        args->thread_id = i;
        args->max_requests = max_requests;
        args->buffer = buffer_ptr;
        PTHREAD_CREATE(&producer_threads[i], producer, (void*)args);
    }
    for (int i = 0; i < num_consumers; i++) {
        ThreadArgs* args = (ThreadArgs*)malloc_or_die(sizeof(ThreadArgs));
        args->thread_id = i;
        args->max_requests = max_requests;
        args->buffer = buffer_ptr;
        PTHREAD_CREATE(&consumer_threads[i], consumer, (void*)args);
    }
    for (int i = 0; i < num_producers; i++) {
        PTHREAD_JOIN(producer_threads[i]);
    }
    for (int i = 0; i < num_consumers; i++) {
        PTHREAD_JOIN(consumer_threads[i]);
    }
    gettimeofday(&end, NULL);
    long elapsed = duration(start, end);

    printf("num_producers: %d\n", num_producers);
    printf("num_consumers: %d\n", num_consumers);
    printf("buffer_size: %d\n", buffer_size);
    printf("max_requests: %d\n", max_requests);
    printf("Elapsed time: %ld microseconds\n", elapsed);
    printf("\nRunning in %s\n\n", NDEBUG_DEFINED ? "Release (NDEBUG defined; e.g. assertion statements removed)" : "Debug (NDEBUG not defined; e.g. assertion statements included)");
    
    free(producer_threads);
    free(consumer_threads);
    destroy_buffer(buffer_ptr);
    MUTEX_DESTROY(&mutex);
    return 0;
}
