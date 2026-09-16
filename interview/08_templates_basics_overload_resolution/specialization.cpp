#include <iostream>


//class specialization

template<typename T, typename U>
class A {
    T* data;
    U value; 
};

//partial specialization
//ambigious template instantation - generatig code from template
template<typename T>
class A<T, int> {
    T* data; 
    int value; 
};

template<typename T>
class A<int, T> {
    T* data; 
    int value; 
};

//full specialization 
template<>
class A<int, double> {
    int* data; 
    double value; 
}; 




int main() {
    A<int, int> a;
}