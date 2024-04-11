#include <iostream>

struct VectorEnum {
    double* elements;
    int size;
};

void vector_init(VectorEnum& v, int s) {
    v.elements = new double[s];
    v.size = s;
}

void test(VectorEnum v, VectorEnum& vr, VectorEnum* vp) {
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

class VectorClass {
    public:
        VectorClass(int s): elements{new double[s]}, num_elements{s} {}
        // needs to be a reference so we can assign to it as well as read from it.
        // technically we could do double* and return &elements[i] but that would return a pointer which would be difficult to work with
        double& operator[](int i) {return elements[i];}
        int size() {return num_elements;}
    private:
        double* elements;
        int num_elements;
};

int main() {
    int x = 2;
    int y = 3;
    int& r = x;
    int* p = &x;
    printf("x = %d\n", x);
    printf("r = %d\n", r);
    printf("p = %p\n", p);
    printf("*p = %d\n", *p);
    r = 4;
    printf("x = %d\n", x);
    printf("r = %d\n", r);
    printf("p = %p\n", p);
    printf("*p = %d\n", *p);
    int& ry = y;
    r = ry;  // read through r2, write through r; x becomes 3; but y and x are still independent
    printf("x = %d\n", x);
    printf("r = %d\n", r);
    printf("p = %p\n", p);
    printf("*p = %d\n", *p);
    y = 10;    
    printf("x = %d\n", x);
    printf("r = %d\n", r);
    printf("p = %p\n", p);
    printf("*p = %d\n", *p);
    // can't assign a pointer to reference r = p
    x = 20;
    printf("x = %d\n", x);
    printf("r = %d\n", r);
    printf("p = %p\n", p);
    printf("*p = %d\n", *p);
    
    VectorEnum v1;
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

    VectorClass v2(5);
    v2[0] = 1.0;
    v2[1] = 2.0;
    printf("v2.size = %d\n", v2.size());
    printf("v2[0] = %f\n", v2[0]);
    printf("v2[1] = %f\n", v2[1]);
    printf("v2[2] = %f\n", v2[2]);

    return 0;
}
