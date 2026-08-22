#include <iostream>

//VLA
int main(){
    int n; 
    std::cin >> n; 

    //variable length array
    //not accepted by C++ standart 
    //acceptable for backward copibility with C
    //will not compile with -Wpedantic

    //using of this type of arrays is extra overhed in runtime 
    //when we don't now size of the array in compile time - compilator needs in runtime calculate shifts of stack pointer 
    int arr[n]; 
    
}