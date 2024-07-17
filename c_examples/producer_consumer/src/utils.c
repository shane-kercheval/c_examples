#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "utils.h"

long duration(struct timeval start, struct timeval end) {
    return ((end.tv_sec - start.tv_sec) * MICRO_SECONDS_IN_SECOND) + (end.tv_usec - start.tv_usec);
}

void* malloc_or_die(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        perror("malloc");
        exit(1);
    }
    return ptr;
}
