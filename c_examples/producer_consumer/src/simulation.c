/*
* Some of this code has been inspired by and/or modified from `Operating Systems: Three Easy Pieces (p. 383).`
*/
#include "simulation.h"

// volatile int buffer->total_puts = 0; // need volatile to prevent compiler optimizations
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// // OSTEP: "producer threads wait on the condition `not_full`, and signals `not_empty`"
// // OSTEP: "consumer threads wait on the condition `not_empty`, and signals `not_full`"
// pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
// pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;


/*
 * Worker thread function that produces requests.
 * Required format:  void *(*start_routine)(void *)
*/
void* producer(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Buffer* buffer = args->buffer;
    pthread_mutex_t* mutex = args->mutex;
    pthread_cond_t* not_full = args->not_full;
    pthread_cond_t* not_empty = args->not_empty;
    int thread_id = args->thread_id;
    int max_requests = args->max_requests;

    printf("|START| producer %d started\n", thread_id);
    // generate fixed number of requests
    // while (buffer->total_puts < max_requests) {   // this is causing deadlock because buffer->total_puts is not locked
    bool put_success = false;
    while (true) {
        MUTEX_LOCK(mutex);
        while (buffer->count == buffer->size) {
            // When the count is equal to the size of the buffer, the buffer is full.
            // We are going to wait until the not_full condition is signaled.
            // We pass in mutex which will be unlocked while waiting and re-locked when signaled.
            VERBOSE_PRINT("... producer %d waiting\n", thread_id);
            COND_WAIT(not_full, mutex);
        }
        if (buffer->total_puts >= max_requests) {
            // this needs to run before we put the request in the buffer and increment the buffer->total_puts
            // otherwise we will add something to the buffer and then exit the function before
            // signaling the not_empty condition and unlocking
            // I only have this break logic here to stop the program at a certain specified number of requests
            // Additionally, we have to broadcast when we are done because there might be multiple consumers,
            // and if we only called COND_SIGNAL, only one consumer would wake up and the others would be stuck.
            COND_BROADCAST(not_empty);
            MUTEX_UNLOCK(mutex);
            printf("|END| producer %d finished\n", thread_id);
            break;
        }
        HttpRequest request;
        request.request_id = buffer->total_puts;
        sprintf(request.data, "%d", buffer->total_puts);
        put_success = put(buffer, request);
        assert(put_success);  // buffer should not be full
        buffer->total_puts++;
        // Signal the not_empty condition to wake up any waiting consumers.
        COND_SIGNAL(not_empty);
        MUTEX_UNLOCK(mutex);
        VERBOSE_PRINT("+++ %d put request %d (put_success: %d)\n", thread_id, request.request_id, put_success);
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
    pthread_mutex_t* mutex = args->mutex;
    pthread_cond_t* not_full = args->not_full;
    pthread_cond_t* not_empty = args->not_empty;
    int thread_id = args->thread_id;
    int max_requests = args->max_requests;
    printf("|START| consumer %d started\n", thread_id);

    while (true) {
        MUTEX_LOCK(mutex);
        while (buffer->count == 0 && buffer->total_puts < max_requests) {
            // When the count is 0, the buffer is empty.
            // We are going to wait until the not_empty condition is signaled.
            // We pass in mutex which will be unlocked while waiting and re-locked when signaled.
            VERBOSE_PRINT("... consumer %d waiting\n", thread_id);
            COND_WAIT(not_empty, mutex);
        }
        if (buffer->count == 0 && buffer->total_puts >= max_requests) {
            // no need to signal the producer threads because they are finished
            // I only have this break logic here to stop the program at a certain specified number of requests
            MUTEX_UNLOCK(mutex);
            printf("|END| consumer %d finished\n", thread_id);
            break;
        }
        HttpRequest request = get(buffer);
        assert(request.request_id != -1);  // buffer should not be empty
        // Signal the not_full condition to wake up any waiting producers.
        COND_SIGNAL(not_full);
        MUTEX_UNLOCK(mutex);
        VERBOSE_PRINT("--- %d consuming request %d\n", thread_id, request.request_id);
        // Simulate request processing
        // printf("Consumer processing request %d: %s\n", request.request_id, request.data);
        // sleep(1);
    }
    return NULL;
}

/**
 * @brief Initializes the arguments for the producer and consumer threads. All producers and
 * consumers share the same buffer, mutex, and condition variables, so we pass pointers to these
 * shared resources to each thread.
 */
ThreadArgs** init_thread_args(int num_producers, int num_consumers, int buffer_size, int max_requests) {
    pthread_mutex_t* mutex = (pthread_mutex_t*)malloc_or_die(sizeof(pthread_mutex_t));
    MUTEX_INIT(mutex);
    // // OSTEP: "producer threads wait on the condition `not_full`, and signals `not_empty`"
    // // OSTEP: "consumer threads wait on the condition `not_empty`, and signals `not_full`"
    pthread_cond_t* not_full = (pthread_cond_t*)malloc_or_die(sizeof(pthread_cond_t));
    COND_INIT(not_full);
    pthread_cond_t* not_empty = (pthread_cond_t*)malloc_or_die(sizeof(pthread_cond_t));
    COND_INIT(not_empty);
    Buffer* buffer = create_buffer(buffer_size);

    ThreadArgs** args_ptrs = (ThreadArgs**)malloc_or_die((num_producers + num_consumers) * sizeof(ThreadArgs*));
    // create producer threads
    for (int i = 0; i < num_producers; i++) {
        args_ptrs[i] = (ThreadArgs*)malloc_or_die(sizeof(ThreadArgs));
        args_ptrs[i]->thread_id = i;
        args_ptrs[i]->max_requests = max_requests;
        args_ptrs[i]->buffer = buffer;
        args_ptrs[i]->mutex = mutex;
        args_ptrs[i]->not_full = not_full;
        args_ptrs[i]->not_empty = not_empty;
    }
    // create consumer threads
    for (int i = 0; i < num_consumers; i++) {
        args_ptrs[num_producers + i] = (ThreadArgs*)malloc_or_die(sizeof(ThreadArgs));
        args_ptrs[num_producers + i]->thread_id = i;
        args_ptrs[num_producers + i]->max_requests = max_requests;
        args_ptrs[num_producers + i]->buffer = buffer;
        args_ptrs[num_producers + i]->mutex = mutex;
        args_ptrs[num_producers + i]->not_full = not_full;
        args_ptrs[num_producers + i]->not_empty = not_empty;
    }
    return args_ptrs;
}

void destroy_thread_args(ThreadArgs** args_ptrs, int num_ptrs) {
    // dosn't matter which args_ptr we use; they all point to the same buffer/mutex/etc.
    destroy_buffer(args_ptrs[0]->buffer);
    MUTEX_DESTROY(args_ptrs[0]->mutex);
    free(args_ptrs[0]->mutex);
    free(args_ptrs[0]->not_full);
    free(args_ptrs[0]->not_empty);
    for (int i = 0; i < num_ptrs; i++) {
        free(args_ptrs[i]);
    }
    free(args_ptrs);
}


SimulationResults simulate(int num_producers, int num_consumers, int buffer_size, int max_requests) {
    ThreadArgs** args_ptrs = init_thread_args(num_producers, num_consumers, buffer_size, max_requests);
    // I need to pass in pointers to the thread functions, so I need to allocate memory for each
    // thread's arguments and store the pointers in an array so I can free them later
    pthread_t* producer_threads = (pthread_t*)malloc_or_die(num_producers * sizeof(pthread_t));
    pthread_t* consumer_threads = (pthread_t*)malloc_or_die(num_consumers * sizeof(pthread_t));

    struct timeval start, end;
    gettimeofday(&start, NULL);
    // launch threads
    for (int i = 0; i < num_producers; i++) {
        printf("||CREATING|| producer %d\n", i);
        PTHREAD_CREATE(&producer_threads[i], producer, (void*)args_ptrs[i]);
    }
    for (int i = 0; i < num_consumers; i++) {
        printf("||CREATING|| consumer %d\n", i);
        PTHREAD_CREATE(&consumer_threads[i], consumer, (void*)args_ptrs[num_producers + i]);
    }
    for (int i = 0; i < num_producers; i++) {
        PTHREAD_JOIN(producer_threads[i]);
    }
    for (int i = 0; i < num_consumers; i++) {
        PTHREAD_JOIN(consumer_threads[i]);
    }
    gettimeofday(&end, NULL);
    long elapsed = duration(start, end);

    int total_puts = args_ptrs[0]->buffer->total_puts;
    int final_buffer_count = args_ptrs[0]->buffer->count;
    free(producer_threads);
    free(consumer_threads);
    destroy_thread_args(args_ptrs, num_producers + num_consumers);
    SimulationResults results = {
        .elapsed_time = elapsed,
        .total_puts = total_puts,
        .final_buffer_count = final_buffer_count
    };
    return results;
}