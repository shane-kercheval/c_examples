#include "utils.h"
#include <string.h>

int utils_function() {
    return 0;
}

/**
 * `strlen` function returns the length of the string without the null terminator. This function is
 * used only to make the code more readable and communicate the intent of the code.
 */
int strlen_null_term(const char *string) {
    return strlen(string) + 1;
}
