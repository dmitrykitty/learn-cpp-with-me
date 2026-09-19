#include <iostream>


//class specialization

template<typename T, typename U>
class A {
    T* data;
    U value; 
};

//partial specialization
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

// ALSO SPECIALIZATION
//--------------------------------------
template <typename T>
class S {}; 

template <typename T>
class S<T&>{ /*other logic*/ };

template <typename T>
class S<const T>{ /* other logic*/ };



int main() {
    A<int, double> a;
}