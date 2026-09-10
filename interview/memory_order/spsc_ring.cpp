#include <atomic> 
#include <thread> 
#include <iostream>

constexpr std::size_t capacity = 8; 

struct Ring {
    std::array<int, capacity> slots{}; 

    std::atomic<std::uint64_t> write_pos{0}; //how many items producer published
    std::atomic<std::uint64_t> read_pos{0}; //hom many items consumer read

};

bool try_push(Ring& ring, int value) {
    const auto write = ring.write_pos.load(std::memory_order_relaxed); //my own cursor
    const auto read = ring.read_pos.load(std::memory_order_acquire); //read other cursor

    if(write - read == capacity) {
        return false; 
    }
    const auto idx = write % capacity; 
    ring.slots[idx] = value; 
    ring.write_pos.store(write + 1, std::memory_order_release); //publish
    return true;
}

bool try_pop(Ring& ring, int& value) {
    const auto read = ring.read_pos.load(std::memory_order_relaxed); 
    const auto write = ring.write_pos.load(std::memory_order_acquire); 

    if(read == write) {
        return false;
    }

    const auto idx = read % capacity; 
    value = ring.slots[idx]; 
    ring.read_pos.store(read + 1, std::memory_order_release);
    return true;
}