#include <iostream>
#include <print>
#include <string>

struct A {
    int a; 
    double b; 
};

struct B {
    int c; 
    double d; 
};

int main(){
    int m = 0; 
    double y = static_cast<double>(m); 

    A a; 
    a.a = 5; 
    a.b = 12.543; 

    B& b = reinterpret_cast<B&>(a); 
    std::println("A.a={} A.b={} B.c={} B.d={}", a.a, a.b, b.c, b.d); 


    int x = 1234535; 
    unsigned char* bytes = reinterpret_cast<unsigned char*>(&x); 

    for(int i = 0 ; i < sizeof(x); ++i){
        std::cout << static_cast<int>(bytes[i]) << ' '; 
    }
}