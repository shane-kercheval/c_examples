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
    if (num_args != 6) {
        printf("Usage: <num_producers> <num_consumers> <buffer_size> <max_requests> <num_simulations>\n");
        return 1;
    }    
    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    int buffer_size = atoi(argv[3]);
    int max_requests = atoi(argv[4]);
    int num_simulations = atoi(argv[5]);
    int consumer_sleep = atoi(argv[6]);

    printf("\n---\nRunning producer-consumer simulation with %d producers, %d consumers, buffer size %d, max requests %d\n", num_producers, num_consumers, buffer_size, max_requests);
    SimulationResults results[num_simulations];
    for (int i = 0; i < num_simulations; i++) {
        results[i] = simulate(num_producers, num_consumers, buffer_size, max_requests, consumer_sleep);
    }

    printf("\n---\nRunning in %s\n\n", NDEBUG_DEFINED ? "Release (NDEBUG defined; e.g. assertion statements removed)" : "Debug (NDEBUG not defined; e.g. assertion statements included)");
    printf("num_producers: %d\n", num_producers);
    printf("num_consumers: %d\n", num_consumers);
    printf("buffer_size: %d\n", buffer_size);
    printf("max_requests: %d\n", max_requests);
    printf("num_simulations: %d\n", num_simulations);
    printf("consumer_sleep: %d\n", consumer_sleep);

    for (int i = 0; i < num_simulations; i++) {
        if (results[i].total_puts != max_requests) {
            fprintf(stderr, "\n**Error**: results[%d].total_puts != max_requests\n\n", i);
        }
        if (results[i].final_buffer_count != 0) {
            fprintf(stderr, "\n**Error**: results[%d].final_buffer_count != 0\n\n", i);
        }
    }
    // printf("Elapsed time: %ld microseconds\n", results.elapsed_time);
    long total_elapsed_time = 0;
    for (int i = 0; i < num_simulations; i++) {
        total_elapsed_time += results[i].elapsed_time;
    }
    double average_elapsed_time = (double)total_elapsed_time / num_simulations;
    printf("Average elapsed time: %.2f microseconds %.2f seconds over %d simulations\n", average_elapsed_time, average_elapsed_time / MICRO_SECONDS_IN_SECOND, num_simulations);
    return 0;
}
