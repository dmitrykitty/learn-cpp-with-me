#include <iostream>

void f(){
    int* pa = new int(); 
    std::cout << pa << '\n'; 
}

int main(){
    while(true){
        f();
    }
}