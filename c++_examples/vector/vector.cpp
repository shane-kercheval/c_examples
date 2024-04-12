#include "vector.h"
#include <iostream>

// Constructor definition
Vector::Vector(int size) : elements{new double[size]}, size_{size} {}

// Destructor to prevent memory leak
Vector::~Vector() {
    delete[] elements;
}

// size method definition
int Vector::size() {
    return size_;
}

// Operator[] definition
double& Vector::operator[](int index) {
    return elements[index];
}

// Operator== definition
bool operator==(Vector& a, Vector& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}
