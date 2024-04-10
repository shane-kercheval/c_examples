#include <iostream>

struct Vector {
    double* elements;
    int size;
};

void vector_init(Vector& v, int s) {
    v.elements = new double[s];
    v.size = s;
}

void test(Vector v, Vector& vr, Vector* vp) {
    // This modifies the original vector because even though v is passed by value, the
    // elements property is a pointer to the original array. So, the original array is modified.
    v.elements[0] = 10;
    // This will not modify the `size` property v is passed by value.
    // If vr or vp were to modify the size property, it would modify the original vector.
    v.size = 100;
    vr.elements[1] = 20;
    // This would modify size since it is passed by reference.
    // vr.size = 200;
    vp->elements[2] = 30;
    // This would modify size since it is passed by pointer.
    // vp->size = 300;
}

int main() {
    Vector v1;
    vector_init(v1, 5);
    v1.elements[0] = 1.0;
    v1.elements[1] = 2.0;
    printf("v1.size = %d\n", v1.size);
    printf("v1.elements[0] = %f\n", v1.elements[0]);
    printf("v1.elements[1] = %f\n", v1.elements[1]);
    printf("v1.elements[2] = %f\n", v1.elements[2]);
    test(v1, v1, &v1);
    printf("v1.size = %d\n", v1.size);
    printf("v1.elements[0] = %f\n", v1.elements[0]);
    printf("v1.elements[1] = %f\n", v1.elements[1]);
    printf("v1.elements[2] = %f\n", v1.elements[2]);
    return 0;
}
