#include "utils.h"
#include <string.h>
#include <stdio.h>

void server_function() {
    printf("Server function called.\n");
}

int main() {
    server_function();
    // int response = utils_function();
    // printf("Utils function returned %d.\n", response);
    char message[255];
    snprintf(message, sizeof(message), "Hello, World!");
    printf("`%s`\n", message);
    printf("Message length: %d\n", (int)strlen(message));
    return 0;
}
