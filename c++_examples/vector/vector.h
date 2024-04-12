#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

class Vector {
public:
    Vector(int size);
    ~Vector();
    double& operator[](int index);
    int size();

private:
    double* elements;
    int size_;
};

// Operator overload declaration
bool operator==(Vector& a, Vector& b);

#endif // VECTOR_H
