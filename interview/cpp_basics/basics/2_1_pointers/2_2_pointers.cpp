#include <iostream>

int main(){
    int a = 0; 
    int* pa = &a; 
    std::cout << pa << std::endl; 

    int** ppa = &pa; 
    printf("ppa: %p, *ppa: %p, **ppa: %d\n", ppa, *ppa, **ppa); 
    // ppa   = address of pa
    // *ppa  = value of pa = address of a
    // **ppa = value of a

    // &a / pa / *ppa = 0x7ffc8cc3fcf4
    // &pa / ppa      = 0x7ffc8cc3fcf8
    /*
    0x...fcf4            int a begins here
    0x...fcf5
    0x...fcf6
    0x...fcf7            int a ends here

    0x...fcf8            int* pa begins here
    0x...fcf9
    0x...fcfa
    0x...fcfb
    0x...fcfc
    0x...fcfd
    0x...fcfe
    0x...fcff            int* pa ends here
    */
    std::cout << sizeof(int*) << std::endl; 
}