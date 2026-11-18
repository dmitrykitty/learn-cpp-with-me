#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cassert> 

/*
shared mutex has two modes: for writers and readers
writers lock it with unique_lock, lock_guard, scoped lock like normal mutex 
readers lock it with shared lock (lock_shared() | try_lock_shared() | unlock_shared())
*/

class FairValueCache {
    using Compute = std::function<long(int)>;      // id -> fair value, slow

    mutable std::shared_mutex m_ ;
    std::unordered_map<int, long> cache_; 
    Compute compute_; 
    int compute_counter{}; 

public:
    explicit FairValueCache(Compute compute): compute_(std::move(compute)){}

    long get_or_compute(int id) {
        {
            std::shared_lock read(m_); 
            auto it = cache_.find(id); 

            if(it != cache_.end()) {
                return it->second; 
            }
        }

        long long value = compute_(id); 
        std::unique_lock write(m_); 
        compute_counter++; 
        auto [it, inserted] = cache_.try_emplace(id, value); //do not overwrite if already was inserted 
        return it->second; 
    }  
    
    // never computes; nullopt if not cached
    std::optional<long> get(int id) const {
        std::shared_lock read(m_); 
        if(auto it = cache_.find(id); it != cache_.end()) {
            return it->second; 
        }
        return std::nullopt; 
    } 

    bool invalidate(int id) {
        std::unique_lock write(m_); 
        if(auto it = cache_.find(id); it != cache_.end()) {
            cache_.erase(it); 
            return true; 
        }
        return false; 
    }

    std::size_t size() const {
        std::shared_lock read(m_); 
        return cache_.size(); 
    }

    // how many times compute ran in total
    int compute_calls() const {
        std::shared_lock read(m_); 
        return compute_counter; 
    }                     
};

class ValueCache {
    using Compute = std::function<long(int)>;      // id -> fair value, slow

    struct Entry {
        std::once_flag once; 
        long value; 
        std::atomic<bool> ready; 
    };

    mutable std::shared_mutex m_ ;
    std::unordered_map<int, std::shared_ptr<Entry>> cache_; 

    Compute compute_; 
    std::atomic<int> compute_counter{}; 

public:
    explicit ValueCache(Compute compute): compute_(std::move(compute)){}

    long get_or_compute(int id) {
        std::shared_ptr<Entry> entry; 
        {
            std::shared_lock read(m_); 
            auto it = cache_.find(id); 

            if(it != cache_.end()) {
                entry = it->second; 
            }
        }
        if(!entry) {
            std::unique_lock write(m_); 
            auto [it, inserted] = cache_.try_emplace(id, std::make_shared<Entry>()); //do not overwrite if already was inserted 
            entry = it->second; //each thread gets the same entry as first that was inserted
        }


        std::call_once(entry->once, [&] {
            entry->value = compute_(id); 
            entry->ready = true; 
            compute_counter++; 
        });

        return entry->value; 
    }  
    
    // never computes; nullopt if not cached
    std::optional<long> get(int id) const {
        std::shared_lock read(m_); 
        if(auto it = cache_.find(id); it != cache_.end() && it->second->ready) {
            return it->second->value; 
        }
        return std::nullopt; 
    } 

    bool invalidate(int id) {
        std::unique_lock write(m_); 
        if(auto it = cache_.find(id); it != cache_.end()) {
            cache_.erase(it); 
            return true; 
        }
        return false; 
    }

    std::size_t size() const {
        std::shared_lock read(m_); 
        return cache_.size(); 
    }

    // how many times compute ran in total
    int compute_calls() const {
        return compute_counter.load(); 
    }                     
}; 


int main() {
    FairValueCache cache([](int id) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return static_cast<long>(id) * 2;
    });

    std::vector<std::thread> readers;
    for (int t = 0; t < 8; ++t) {
        readers.emplace_back([&cache] {                    // capture by reference: one shared cache
            for (int round = 0; round < 20; ++round) {
                for (int id = 0; id < 100; ++id) {
                    long v = cache.get_or_compute(id);
                    assert(v == id * 2);                   // check the result inside the thread
                }
            }
        });
    }
    for (auto& t : readers) t.join();
    std::cout << cache.compute_calls() << " " << cache.size() << '\n'; 


    ValueCache vcache([](int id) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return static_cast<long>(id) * 2;
    });

    std::vector<std::thread> vreaders;
    for (int t = 0; t < 8; ++t) {
        vreaders.emplace_back([&vcache] {                    // capture by reference: one shared cache
            for (int round = 0; round < 20; ++round) {
                for (int id = 0; id < 100; ++id) {
                    long v = vcache.get_or_compute(id);
                    assert(v == id * 2);                   // check the result inside the thread
                }
            }
        });
    }
    for (auto& t : vreaders) t.join();
    std::cout << vcache.compute_calls() << " " << vcache.size(); 
}