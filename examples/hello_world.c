#include <stdio.h> 
#include <stdlib.h>

int main(void) {
    int num_chars = printf("%s\n", "Hello, world!");
    printf("Number of characters printed: %d\n", num_chars);
    if (num_chars <= 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
