#include "utils.h"
#include "unity.h"
#include <string.h>

void test_strlen_null_term() {
    TEST_ASSERT_EQUAL_INT(strlen_null_term((char*)""), 1);
    TEST_ASSERT_EQUAL_INT(strlen_null_term((char*)"123456"), 7);
    TEST_ASSERT_EQUAL_INT(strlen_null_term((char*)"123456"), strlen((char*)"123456") + 1);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_strlen_null_term);
    return UNITY_END();
}
