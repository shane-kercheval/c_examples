#ifndef SIMULATE_H
#define SIMULATE_H

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include "utils.h"
#include "buffer.h"

#ifdef VERBOSE
// https://gcc.gnu.org/onlinedocs/gcc-7.5.0/cpp/Variadic-Macros.html
#define VERBOSE_PRINT(...) printf(__VA_ARGS__)
#else
#define VERBOSE_PRINT(...) /* nothing */
#endif

typedef struct {
    int thread_id;
    int max_requests;
    Buffer* buffer;
    pthread_mutex_t* mutex;
    pthread_cond_t* not_full;
    pthread_cond_t* not_empty;
} ThreadArgs;

typedef struct {
    long elapsed_time;
    int total_puts;
    int final_buffer_count;
} SimulationResults;

void* producer(void* arg);
void* consumer(void* arg);
SimulationResults simulate(int num_producers, int num_consumers, int buffer_size, int max_requests);

#endif // SIMULATE_H
