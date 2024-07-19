#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

#define MICRO_SECONDS_IN_SECOND 1000000

/*
 * Wrapper around functions that need to check for NULL.
 * If NDEBUG (No Debug) is defined (i.e. built in Release mode), assertion statements will be
 * removed, which also removes the expression being evaluated. In Release we still need to execute
 * the functions, but without the overhead of the assert statements.
 */
#ifdef NDEBUG
    #define PTHREAD_CREATE(thread, start_routine, arg) pthread_create(thread, NULL, start_routine, arg)
    #define PTHREAD_JOIN(thread) pthread_join(thread, NULL)

    #define MUTEX_LOCK(mutex) pthread_mutex_lock(mutex)
    #define MUTEX_UNLOCK(mutex) pthread_mutex_unlock(mutex)
    #define MUTEX_INIT(mutex) pthread_mutex_init(mutex, NULL)
    #define MUTEX_DESTROY(mutex) pthread_mutex_destroy(mutex)

    #define COND_INIT(cond) pthread_cond_init(cond, NULL)
    #define COND_SIGNAL(cond) pthread_cond_signal(cond)
    #define COND_BROADCAST(cond) pthread_cond_broadcast(cond)
    #define COND_WAIT(cond, mutex) pthread_cond_wait(cond, mutex)
#else
    #define PTHREAD_CREATE(thread, start_routine, arg) assert(pthread_create(thread, NULL, start_routine, arg) == 0)
    #define PTHREAD_JOIN(thread) assert(pthread_join(thread, NULL) == 0)

    #define MUTEX_LOCK(mutex) assert(pthread_mutex_lock(mutex) == 0)
    #define MUTEX_UNLOCK(mutex) assert(pthread_mutex_unlock(mutex) == 0)
    #define MUTEX_INIT(mutex) assert(pthread_mutex_init(mutex, NULL) == 0)
    #define MUTEX_DESTROY(mutex) assert(pthread_mutex_destroy(mutex) == 0)

    #define COND_INIT(cond) assert(pthread_cond_init(cond, NULL) == 0)
    #define COND_SIGNAL(cond) assert(pthread_cond_signal(cond) == 0)
    #define COND_BROADCAST(cond) assert(pthread_cond_broadcast(cond) == 0)
    #define COND_WAIT(cond, mutex) assert(pthread_cond_wait(cond, mutex) == 0)
#endif

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
