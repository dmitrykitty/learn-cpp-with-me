#include <iostream>

//2D arrays
//1
void f(int**){}
//2
void f(int(*)[5]){}
//3
void f(int* [5]){}
//1 and 3 is the same because array destroys to pointer as argumetn to function

int main(){

    int a[5][5]; //array of 5 arrays of length 5
    //what is more effective: a[5][5] or a[25]. I guess second one, by why? It has different type of memory storage? 
    int* b[5]; //array of 5 pointers to int 
    int (*c)[5]; //pointer of array to 5 ints

    //so int* c and int (*c) are two different types 

}