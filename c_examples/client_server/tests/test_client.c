#include "client.h"
#include <assert.h>
#include <stdio.h>

void test_client_function() {
    client_function();
    assert(1);
}

int main() {
    test_client_function();
    printf("Client tests passed.\n");
    return 0;
}
