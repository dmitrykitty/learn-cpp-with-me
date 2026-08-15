#include <iostream>
#include <vector>

class A {

};

int main() {
    //Three ways to initialize variable / object 
    int x(0);
    int y = 0;
    int z{0}; //the same as z = {0}

    //========================

    A a1; //call default constructor, we can call =default constructor also with a1{}. Even with initializer list default construcotr for {} will be called. To call constructor with initializer list with now arguments use A({}) where {} - empty initializer list 
    A a2 = a1; //call copy constructor 
    a2 = a1; //call assigment operator 
    //initialization with {} called uniform or braced
    //if at least one constructor has initializer list as argument - every initialization with {} will call this copnstructor 

    A a4(); //no default constructor, just function 

    //the same works for vector 
    std::vector<int> v1(3, 10); // {10, 10, 10}
    std::vector<int> v2{3, 10}; // {3, 10}
}