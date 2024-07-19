/*
* This is a simulation of a producer-consumer problem using pthreads. The simulate function creates
* a buffer and producer/consumer threads and waits for them to finish. The producer function
* generates "requests" and puts them in the buffer, while the consumer function gets the requests
* from the buffer and processes them.
* 
* Some of this code has been inspired by and/or modified from `Operating Systems: Three Easy Pieces (p. 383).`
*/
#include <stdio.h>
#include <unistd.h>
// #include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "utils.h"
#include "buffer.h"
#include "simulation.h"

// NDEBUG (No Debug) is defined in Release mode and removes assertion statements
#ifdef NDEBUG
    bool NDEBUG_DEFINED = true;
#else
    bool NDEBUG_DEFINED = false;
#endif


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

    printf("\n---\nRunning producer-consumer simulation with %d producers, %d consumers, buffer size %d, max requests %d\n", num_producers, num_consumers, buffer_size, max_requests);

    SimulationResults results = simulate(num_producers, num_consumers, buffer_size, max_requests);

    printf("\n---\nRunning in %s\n\n", NDEBUG_DEFINED ? "Release (NDEBUG defined; e.g. assertion statements removed)" : "Debug (NDEBUG not defined; e.g. assertion statements included)");
    printf("num_producers: %d\n", num_producers);
    printf("num_consumers: %d\n", num_consumers);
    printf("buffer_size: %d\n", buffer_size);
    printf("max_requests: %d\n", max_requests);
    printf("Final Requests Generated: %d\n", results.total_puts);
    if (results.total_puts != max_requests) {
        fprintf(stderr, "\n**Error**: buffer_ptr->total_puts != max_requests\n\n");
    }
    printf("Final Buffer Count: %d\n", results.final_buffer_count);
    if (results.final_buffer_count != 0) {
        fprintf(stderr, "\n**Error**: results.final_buffer_count != 0\n\n");
    }
    printf("Elapsed time: %ld microseconds\n", results.elapsed_time);
    return 0;
}
