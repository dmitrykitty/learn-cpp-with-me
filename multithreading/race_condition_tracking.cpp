#include <thread>
#include <iostream>

int counter = 0; 

void increment() {
    for(int i = 0; i < 1'000'000; ++i) {
        counter++; 
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    std::thread t3(increment);
    std::thread t4(increment);

    t1.join(); 
    t2.join(); 
    t3.join(); 
    t4.join(); 

    std::cout << counter; 
}