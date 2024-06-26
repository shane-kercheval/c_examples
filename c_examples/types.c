#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>


int main() {
    printf("Size of unsigned int: %d bytes\n", (int) sizeof(unsigned int));
    printf("Max value of unsigned int: %u\n", (unsigned int) -1);

    // storing integers in a char variable in order to reduce memory usage
    unsigned char a = 255;
    printf("The size of a is: %lu byte(s)\n", sizeof(a));
    printf("The value of a is: int=%d char='%c'\n", a, a);
    a = a + 1; // 255 + 1 = 0
    assert(a == 0);
    printf("The value of a is: int=%d char='%c'\n", a, a);
    a = 'A';
    printf("The value of a is: int=%d char='%c'\n", a, a);

    typedef unsigned char byte;
    byte b = -1;
    a = -1;
    assert(a == b);
    printf("The size of b is: %lu byte(s)\n", sizeof(b));
    printf("The value of b is: int=%d char='%c'\n", b, b);


    typedef enum {
        sun, mon, tue, wed, thu, fri, sat
    } DayEnum;
    // In this example, DayEnum is defined as a typical enum.
    // Then, Day is defined as an unsigned char, allowing it to store the enum values (e.g. Day)
    // while only using a single byte of memory.
    // This approach offers a compromise: it utilizes enums for clearer code and explicitly controls memory usage by specifying the variable's type as unsigned char.
    typedef unsigned char Day;  // via DayEnum
    const Day day_values[] = {sun, mon, tue, wed, thu, fri, sat};

    Day today = mon;
    printf("Value of today: %d\n", today);
    printf("Size of today: %lu byte(s)\n", sizeof(today));
    Day days[7];// = {sun, mon, tue, wed, thu, fri, sat};
    days[0] = sun;
    assert(days[0] == *days);
    printf("Value of days[1]: %d\n", days[1]);
    days[1] = mon;
    assert(days[1] == *(days + 1));
    printf("Value of days[1]: %d\n", days[1]);
    printf("Value of days[1]: %d\n", *(days + 1));
    days[2] = tue;
    days[3] = wed;
    days[4] = thu;
    days[5] = fri;
    days[6] = sat;
    days[100] = sun; // days[7] is out of bounds, but the program will not crash
    printf("Size of days: %lu byte(s)\n", sizeof(days));

    // enum day2 {sun2, mon2, tue2, wed2, thu2, fri2, sat2};
    typedef enum {sun2, mon2, tue2, wed2, thu2, fri2, sat2} day2;
    // enum day2 today2 = tue2;
    day2 today2 = tue2;
    printf("Value of today2: %d\n", today2);
    printf("Size of today2: %lu byte(s)\n", sizeof(today2));
    // enum day2 days2[7] = {sun2, mon2, tue2, wed2, thu2, fri2, sat2};
    day2 days2[7] = {sun2, mon2, tue2, wed2, thu2, fri2, sat2};
    printf("Size of days: %lu byte(s)\n", sizeof(days2));


    // limit of signed char
    printf("The min value of signed char: %d\n", SCHAR_MIN);
    printf("The max value of signed char: %d\n", SCHAR_MAX);
    // this produces a warning when compiling but successfully compiles and runs
    signed char c = SCHAR_MIN - 1;
    printf("The value of c is: %d\n", c);
    assert(c == SCHAR_MAX);

    return 0;
}
