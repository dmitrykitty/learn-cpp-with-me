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
    int* data; 
    T value; 
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

template <typename T>
class S<T*> { /*...*/};


//=================================================================
// A full specialization is attached to one particular primary function template. Overload resolution first chooses the primary template overload, and only then the compiler uses its specialization if one exists
//function specialization (only full specialization)
//for f(int, int) -> 2
template <typename T, typename U>
void f(T, U) {
    std::cout << 1; 
}

template <>
void f(int, int) {
    std::cout << 3; 
}

template <typename T>
void f(T, T) {
    std::cout << 2; 
}
//-----------------------
//the same function overloading but different order
// k(int int) -> 3
template <typename T, typename U>
void k(T, U) {
    std::cout << 1; 
}

template <typename T>
void k(T, T) {
    std::cout << 2; 
}

template <>
void k(int, int) {
    std::cout << 3; 
}

//firstly most specified overloading selected. for f it is f(T, T).Because no specialization of template<typename T> provided
//only possible selection f(T, T)

//what's going with k? Firstly template<typename T> selected, i think it's specialization instantiated and now compiler selects between two specializations 
//of course k(int, int) is much more specific then k(T, T) so -> 3 is printed
// function templates
//       |
//       +-- overload resolution between primary templates
//       |
//       +-- selected primary
//               |
//               +-- use explicit specialization if one exists

namespace EX1 {
template<typename T, typename U>
struct A {
    static void f() {
        std::cout << 1;
    }
};

template<typename T>
struct A<T, int> {
    static void f() {
        std::cout << 2;
    }
};

template<>
struct A<double, int> {
    static void f() {
        std::cout << 3;
    }
};

int main() {
    A<float, double>::f(); //1
    A<float, int>::f(); //2
    A<double, int>::f();//3
}

} //ex1

//no template specialization wins because as <T, int> the same as <int, T> fits <int, int> params
namespace EX2 {
template<typename T, typename U>
struct A {};

template<typename T>
struct A<T, int> {};

template<typename T>
struct A<int, T> {};

int main() {
    A<int, int> x;
}
} //ex2

namespace EX3 {
template<typename T>
struct S {
    static void f() {
        std::cout << 1;
    }
};

template<typename T>
struct S<T*> {
    static void f() {
        std::cout << 2;
    }
};

template<typename T>
struct S<const T*> {
    static void f() {
        std::cout << 3;
    }
};

int main() {
    S<const int*>::f(); //3, T == int
}    
}//ex3

namespace EX4 {
template<typename T, typename U>
void f(T, U) {
    std::cout << 1;
}

template<>
void f(int, int) {
    std::cout << 3;
}

template<typename T>
void f(T, T) {
    std::cout << 2;
}

int main() {
    f(1, 1); //f(int, int) -> best overloaded version f(T, T) -> no spec found -> 2
}
}//ex4

namespace EX5 {
template<typename T>
void g(T) {
    std::cout << 1;
}

template<>
void g<int>(int) {
    std::cout << 2;
}

void g(int) {
    std::cout << 3;
}

int main() {
    g(42); //if it possible - version without template will be selected -> 3
}
} //ex5

int main() {
    A<int, double> a;
}