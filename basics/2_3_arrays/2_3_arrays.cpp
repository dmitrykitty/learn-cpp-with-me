#include <iostream>

//2.3. Arrays
void f(int* a); 

int main(){

    int a[5]; //array with 5 random numbers 
    int b[] = {1, 2, 3, 4, 5}; // array with 1 2 3 4 5
    int c[5] = {}; // or int a[]{}  0 0 0 0 0
    int d[5] = {1, 2}; //1 2 0 0 0 

    //int e[50'000'000]; //seg fault 
    static int e[50'000'000]; //save array to .data for static memory 

    //ARRAYS AS POINTERS
    int arr[]{1, 2, 3, 4, 5}; //arr is a pointer to first element
    std::cout << *(arr + 2) << std::endl; //3 (array to pointer conversion)

    //a[n] == *(a + n) so n[a] is correct form 

    int* parr = arr + 3; 
    std::cout << parr[-1] << std::endl; //3

    //unsupported operations for arrays
    //a = b
    // a++
    // a+= 1

    int* pa = a; 
    std::cout << "pa: " << sizeof(pa) << " , a: " << sizeof(a) << std::endl; //pa: 8 (size of pointer) , a: 20 (size of all elements in the array)
}

//we can overload this function with void f(int a[3]) -> redefinition. so int a[3] = int* a as argument to function 
void f(int* a){
    std::cout << a[2] << std::endl; 
}
