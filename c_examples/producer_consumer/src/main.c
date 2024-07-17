#include <stdio.h>
#include <unistd.h>
// #include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "utils.h"

// NDEBUG is defined in Release mode
#ifdef NDEBUG
    bool NDEBUG_DEFINED = true;
#else
    bool NDEBUG_DEFINED = false;
#endif


int main(int argc, char *argv[]) {
    // printf("Num args: %d\n", argc);
    // for (int i = 0; i < argc; i++) {
    //     printf("Arg %d: %s\n", i, argv[i]);
    // }
    printf("\n\nRelease Mode (NDEBUG DEFINED): %s\n\n", NDEBUG_DEFINED ? "true" : "false");
    printf("hello\n");
    struct timeval start, end;
    gettimeofday(&start, NULL);
    sleep(1);
    gettimeofday(&end, NULL);
    long elapsed = duration(start, end);
    printf("Elapsed time: %ld microseconds\n", elapsed);
    printf("goodbye\n");

    return 0;
}
