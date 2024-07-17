#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

#define MICRO_SECONDS_IN_SECOND 1000000

/*
 * Wrapper around functions that need to check for NULL.
 * `assert` statements will be removed if NDEBUG is defined (i.e. built in Release mode).
 */
// #define MALLOC(size) ({ void *ptr = malloc(size); assert(ptr != NULL); ptr; })

/*
 * Calculate the duration between two timevals in microseconds.
 * For example:
 *   #include <sys/time.h>
 *   #include <stdio.h>
 *   #include "utils.h"
 *   struct timeval start, end;
 *   gettimeofday(&start, NULL);
 *   // do something
 *   gettimeofday(&end, NULL);
 *   long elapsed = duration(start, end);
 *   printf("Elapsed time: %ld microseconds\n", elapsed);
*/
long duration(struct timeval start, struct timeval end);

/*
 * Wrapper around malloc that checks for NULL.
*/
void *malloc_or_die(size_t size);

#endif // UTILS_H
