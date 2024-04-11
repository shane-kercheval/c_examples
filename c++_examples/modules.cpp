export module hello; // Declares the module named hello
import <iostream>;

export void say_hello() { // Exporting say_hello function so it can be used outside the module
    std::cout << "Hello from module!" << std::endl;
}

export class Vector {
    public:
        Vector(int size);
        double& operator[](int index);
        int size();
    private:
        double* elements;
        int size_;
};

Vector::Vector(int size):
    elements {new double[size]},
    size_ {size}
{}

int Vector::size() {
    return size_;
}

double& Vector::operator[](int index) {
    return elements[index];
}

export bool operator==(Vector& a, Vector& b) {
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


