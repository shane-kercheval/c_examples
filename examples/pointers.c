#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

void swap(int *pa, int *pb){
    // &x gets the address of x.
    // (void*)&x casts the address to void*, which is a generic pointer type in C.
    // This casting is done because %p expects a void* type.
    // printf prints the memory address of x using %p.
    printf("The address of pa is:       %p\n", (void*)pa);
    printf("The address of &pa is:      %p\n", (void*)&pa);
    printf("The address of *&pa is:     %p\n", (void*)*&pa);
    assert(pa == *&pa);

    int t = *pa;  // pa is the address of a, *pa is the value of a
    // *pa is the value of pa,
    // but more specifically it is the thing that's stored at the address pa
    // so *pa = *pb means that we are storing the value of pb at the address of pa
    // which changes the value of a (in main, which points to the same address) to the value of b
    *pa = *pb;
    *pb = t;
    printf("swap: *pa = %d, *pb = %d\n", *pa, *pb);
}

int main(void) {
    int a = 21;
    int b = 17;

    printf("The address of a (&a) is:   %p\n", (void*)&a);
    printf("main: a = %d, b = %d\n", a, b);
    swap(&a, &b);
    printf("main: a = %d, b = %d\n", a, b);
    assert(a == 17);
    assert(b == 21);

    bool x = true;
    assert((a == 17) == x);

    return 0;
}
