#include <iostream>

int main(){
    int a = 1; 
    int* pa = &a; 

    {
        int b = 2; 
        pa = &b; 
    }

    std::cout << pa << ' ' << *pa << std::endl; //b already dead but pa still exists. *pa UB but possibly 2; 0x7ffe7edd246c 2

    int c = 3, d = 4, e = 5, f = 6; 
    std::cout << &c << ' ' << &d << ' ' << &e << ' ' << &f; //some of the variables could already be stored in 0x7ffe7edd246c
}