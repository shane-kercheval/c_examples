#include "utils.h"
#include "unity.h"

void test_utils_function() {
    int result = utils_function();
    TEST_ASSERT_TRUE(result == 0);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_utils_function);
    return UNITY_END();
}
