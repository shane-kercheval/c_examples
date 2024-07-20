#include "utils.h"
#include <stdio.h>

void server_function() {
    printf("Server function called.\n");
}

int main() {
    server_function();
    int response = utils_function();
    printf("Utils function returned %d.\n", response);
    return 0;
}
