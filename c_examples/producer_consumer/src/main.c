#include <stdio.h>
#include <unistd.h>
// #include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "utils.h"
#include "buffer.h"

// NDEBUG is defined in Release mode
#ifdef NDEBUG
    bool NDEBUG_DEFINED = true;
#else
    bool NDEBUG_DEFINED = false;
#endif


int main(int argc, char *argv[]) {
    // first argument is the number of producer threads
    // second argument is the number of consumer threads
    // third argument is the size of the buffer
    printf("\n\nRelease Mode (NDEBUG DEFINED): %s\n\n", NDEBUG_DEFINED ? "true-Release" : "false-Debug");
    
    int num_args = argc - 1;
    if (num_args != 3) {
        printf("Usage: %s <num_producers> <num_consumers> <buffer_size>\n", argv[0]);
        return 1;
    }    
    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    int buffer_size = atoi(argv[3]);
    printf("num_producers: %d\n", num_producers);
    printf("num_consumers: %d\n", num_consumers);
    printf("buffer_size: %d\n", buffer_size);
    
    pthread_t *producer_threads = (pthread_t *)malloc_or_die(num_producers * sizeof(pthread_t));
    pthread_t *consumer_threads = (pthread_t *)malloc_or_die(num_consumers * sizeof(pthread_t));

    // need to define this globally or pass it as an argument to the producer and consumer functions
    // so it would need to be defined on the heap
    Buffer *buffer = create_buffer(buffer_size);

    
    // printf("hello\n");
    // struct timeval start, end;
    // gettimeofday(&start, NULL);
    // sleep(1);
    // gettimeofday(&end, NULL);
    // long elapsed = duration(start, end);
    // printf("Elapsed time: %ld microseconds\n", elapsed);
    // printf("goodbye\n");

    free(producer_threads);
    free(consumer_threads);
    destroy_buffer(buffer);
    return 0;
}
