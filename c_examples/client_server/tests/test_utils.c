#include "utils.h"
#include <assert.h>
#include <stdio.h>

void test_utils_function() {
    printf("Running utils test...\n");
    int result = utils_function();
    assert(result == 0);
}

int main() {
    test_utils_function();
    printf("Utils tests passed.\n");
    return 0;
}
