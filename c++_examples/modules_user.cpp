import hello; // Importing the hello module
import <iostream>; // Importing the C standard input/output library

int main() {
    say_hello(); // Calling function from the hello module
    Vector v1(5);
    v1[0] = 1.0;

    printf("v1[0] = %f\n", v1[0]);
    printf("size of v1 = %d\n", v1.size());

    Vector v2(5);
    v2[0] = 1.0;
    bool equal = v1 == v2;
    printf("v1 == v2: %s\n", equal ? "true" : "false");
    return 0;
}
