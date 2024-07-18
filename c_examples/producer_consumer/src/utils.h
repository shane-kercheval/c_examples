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
#define PTHREAD_CREATE(thread, start_routine, arg) assert(pthread_create(thread, NULL, start_routine, arg) == 0);
#define PTHREAD_JOIN(thread) assert(pthread_join(thread, NULL) == 0);

#define MUTEX_LOCK(mutex) assert(pthread_mutex_lock(mutex) == 0);
#define MUTEX_UNLOCK(mutex) assert(pthread_mutex_unlock(mutex) == 0);
#define MUTEX_DESTROY(mutex) assert(pthread_mutex_destroy(mutex) == 0);

#define COND_SIGNAL(cond) assert(pthread_cond_signal(cond) == 0);
#define COND_WAIT(cond, mutex) assert(pthread_cond_wait(cond, mutex) == 0);

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
