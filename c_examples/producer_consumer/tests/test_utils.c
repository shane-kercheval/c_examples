#include <assert.h>
#include <sys/time.h>
#include "utils.h"

void test_duration() {
    struct timeval start, end;
    long result;
    start.tv_sec = 1;
    start.tv_usec = 0;
    
    // 0 seconds later
    end.tv_sec = 1;
    end.tv_usec = 0;
    result = duration(start, end);
    assert(result == 0);

    // half a second later
    end.tv_sec = 1;
    end.tv_usec = 500000;
    result = duration(start, end);
    assert(result == 500000);
}

void test_malloc_or_die() {
    void *ptr = malloc_or_die(10);
    assert(ptr != NULL);
    free(ptr);
}

int main() {
    test_duration();
    test_malloc_or_die();
    return 0;
}
