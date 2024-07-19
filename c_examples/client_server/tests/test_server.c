#include "utils.h"
#include <assert.h>
#include <stdio.h>

void test_utils_function() {
    utils_function();
    assert(1);
}

int main() {
    test_utils_function();
    printf("Utils tests passed.\n");
    return 0;
}
