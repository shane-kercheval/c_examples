#include "vector.h"
#include <iostream>

Vector::Vector(int size) : elements{new double[size]}, size_{size} {}

Vector::~Vector() {
    delete[] elements;
}

int Vector::size() {
    return size_;
}

double& Vector::operator[](int index) {
    return elements[index];
}

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
