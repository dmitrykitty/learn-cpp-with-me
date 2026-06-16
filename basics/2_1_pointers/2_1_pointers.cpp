#include <iostream>
#include <vector>

//II. Compound types

//2.1. Pointers 

int main(){

    int x = 0; 
    int* px; //here * is part of type int* (pointer)
    px = &x; // &: T -> T* here & is operator
    int y = *px; // *: T* -> T here * is operator (dereferencing) 

    std::cout << px << std::endl; 

    //for multiple pointer declaration 
    int a = 0, b = 1; 
    int* pa = &a, *pb = &b; //* for each variable 
    std::cout << pa << ' ' << pb << std::endl; 

    std::cout << *(pa + 1) << std::endl; // +: (T*, int) -> T* + sizeof(T) * int
    //allowed operations + int, - int, ++, --, ptr - ptr, <, >, <=, >=, ==
    //ptr - ptr works like (address1 - address2) / sizeof(T)
    //but would be error if we compare or differencing pointers of two different types 

    std::vector<int> v = {1, 2, 3, 4, 5};
    int* pv = &v[0]; 
    std::cout << *pv << ' ' << *(pv + 1) << ' ' << *pv++ <<  std::endl; 

    int* pvlast = &v[4]; 
    std::cout << pvlast - pv << std::endl; 

    //*p - lvalue, so we can *p = 1 works
    //& as agrument needs lvalue, so &a++ will not work and &(++a) will work 

    //----------------void*------------------
    int l = 2; 
    void* pl = &l; 
    std::cout << *(int*)pl <<std::endl; //why 0? 

    //---------------nullptr & NULL------------------
    /*
    NULL - constant MAKROS? 
    nullptr_t nullptr - key word but what is it? 
    */
}