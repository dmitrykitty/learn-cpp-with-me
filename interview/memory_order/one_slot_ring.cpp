#include <atomic>
#include <iostream>
#include <thread>

constexpr int iterations = 1000000;

//without explicit memory order -> std::memory_order_seq_cst used
//it gives very strong garuantees and make operations sequantional
//it gives as structured order for multiple operations between multiple threads  

//for release and acquire it guarantees that producer write HAPPEN BEOFRE consumer read 

struct OneSlot {
    int value{0}; 
    std::atomic<bool> full{false};
};

void producer(OneSlot& slot) {
    for(int i = 1; i <= iterations; i++) {
        while(slot.full.load(std::memory_order_acquire)) {
            continue;
        }

        slot.value++; 
        slot.full.store(true, std::memory_order_release); 
    }
}

void consumer(OneSlot& slot) {
    for(int expected = 1; expected <= iterations; expected++) {
        while(!slot.full.load(std::memory_order_acquire)) {
            continue;
        }

        if(expected != slot.value) {
            std::cout << "ERROR"; 
            break;
        }

        std::cout << slot.value << " "; 
        slot.full.store(false, std::memory_order_release);

    }
}

int main() {
    OneSlot slot;

    std::thread producer_thread(producer, std::ref(slot));
    std::thread consumer_thread(consumer, std::ref(slot));

    producer_thread.join();
    consumer_thread.join();

    std::cout << "OK\n";
}