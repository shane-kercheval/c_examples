/**
 * https://github.com/ThrowTheSwitch/Unity
 * https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityGettingStartedGuide.md
 */
#include "utils.h"
#include <stdio.h>
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_utils_function() {
    printf("Running utils test...\n");
    int result = utils_function();
    TEST_ASSERT_TRUE(result == 0);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_utils_function);
    return UNITY_END();
}
