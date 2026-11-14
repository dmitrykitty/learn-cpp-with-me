#include <mutex>
#include <thread>
#include <iostream>

int x = 0; 
int y = 0; 
int z = 0;

void race() {
    for(int i = 0; i < 100000; ++i) x++; 
    for(int i = 0; i < 100000; ++i) x--; 
}

std::mutex mfory; 

void not_race() {
    for(int i = 0; i < 100000; ++i) { 
        mfory.lock(); //waiting for mutex in critical section
        //if lock in one thread called twice -> std::system_error
        y++; 
        mfory.unlock();
    } 
    for(int i = 0; i < 100000; ++i) {
        mfory.lock();
        y--;
        mfory.unlock();
    } 
}

std::mutex mforz; 

void not_race_with_lock_guard() {
    for(int i = 0; i < 100000; ++i) { 
        std::lock_guard<std::mutex> lg{mforz}; 
        //RAII wrapper over mutex/ even if exception thrown - mutext will be unlocked in destructor
        z++; 
    } 
    for(int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lg{mforz}; 
        z--;
    } 
}


int main() {
    std::thread t1(race); 
    std::thread t2(race); 

    std::thread t3(not_race); 
    std::thread t4(not_race); 

    std::thread t5(not_race_with_lock_guard); 
    std::thread t6(not_race_with_lock_guard); 

    t1.join(); 
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();

    std::cout << x << " " << y << " " << z; 
}