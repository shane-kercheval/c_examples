#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include "simulation.h"

// https://stackoverflow.com/questions/1067226/c-multi-line-macro-do-while0-vs-scope-block
#define assert_true(condition) do { \
    if (!(condition)) { \
        fprintf( \
            stderr, \
            "Assertion failed: function %s, file %s, line %d.\n", \
            __func__, __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)


void test_simulation() {
    int num_producers = 1;
    int num_consumers = 1;
    int buffer_size = 10;
    int max_requests = 1000;
    SimulationResults results = simulate(num_producers, num_consumers, buffer_size, max_requests, 0);
    assert_true(results.elapsed_time > 0);
    assert_true(results.total_puts == max_requests);
    assert_true(results.final_buffer_count == 0);
}

void test_simulation_multiple_producers_consumers() {
    int num_producers = 5;
    int num_consumers = 5;
    int buffer_size = 10;
    int max_requests = 1000;
    SimulationResults results = simulate(num_producers, num_consumers, buffer_size, max_requests, 0);
    assert_true(results.elapsed_time > 0);
    assert_true(results.total_puts == max_requests);
    assert_true(results.final_buffer_count == 0);
}

void test_simulation_buffer_larger_than_requests() {
    int num_producers = 2;
    int num_consumers = 2;
    int buffer_size = 1000;
    int max_requests = 100;
    SimulationResults results = simulate(num_producers, num_consumers, buffer_size, max_requests, 0);
    assert_true(results.elapsed_time > 0);
    assert_true(results.total_puts == max_requests);
    assert_true(results.final_buffer_count == 0);
}

int main() {
    test_simulation();
    test_simulation_multiple_producers_consumers();
    test_simulation_buffer_larger_than_requests();
    return 0;
}
