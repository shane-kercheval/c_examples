#include <iostream>

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
    VectorClass v1(5);
    v1[0] = 1.0;
    v1[1] = 2.0;
    std::cout << "v1.size = " << v1.size() << std::endl;
    std::cout << "v1.elements[0] = " << v1[0] << std::endl;
    std::cout << "v1.elements[1] = " << v1[1] << std::endl;
    std::cout << "v1.elements[2] = " << v1[2] << std::endl;
    return 0;   
}