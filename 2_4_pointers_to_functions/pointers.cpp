#include <iostream>
#include <algorithm>

bool cmp(int x, int y){
    return x * x < y * y;
}

void f(int){}
void f(double){}

int main(){

    //implementation of the function stored as binary code in text
    //so pointer to function is simply pointer to the area where its binary code saved

    int a[] = {1, -5, 2, 9, -3}; 
    std::sort(a, a + 5, cmp);
    //why cmp and &cmp is the same? 

    //what is type of &cmp? 
    //pointer to the function which takes two ints and returns bool
    bool (*p)(int, int) = cmp; 
    std::cout << (void*)p << std::endl; //withoout casting we will get 0 or 1. why? 

    for(int i = 0; i < 5; i++){
        std::cout << a[i] << " "; 
    }

    void (*p1)(int) = f; //will it work? here type of f is not defined and depends on what we have on the left side. how does it work?
    //if function is not defined - we cant take it address -> we will get linker exception 
}
