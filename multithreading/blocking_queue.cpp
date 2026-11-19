#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>
#include <cassert> 
#include <iostream>
#include <thread>

template <typename T> 
class BlockingQueue {
    std::mutex m_; 
    std::condition_variable not_empty_, not_full_; 
    bool closed_{false}; 
    std::size_t capacity_, read_idx_{0}, write_idx_{0}, size_{0}; 
    

    std::vector<std::optional<T>> buffer_; 
public: 
    explicit BlockingQueue(std::size_t capacity): capacity_(capacity), buffer_(capacity) {
        assert(capacity != 0); 
    } 

    // blocks while full; returns false if the queue is closed
    bool push(T value) {
        {
            std::unique_lock lock(m_); 
            not_full_.wait(lock, [&] { return size_ < capacity_ || closed_; }); 

            if(closed_) return false; 

            buffer_[write_idx_].emplace(std::move(value)); 
            write_idx_ = (write_idx_ + 1) % capacity_; 
            size_++; 
        }

        not_empty_.notify_one(); 
        return true;
    }   
    
    // blocks while empty; returns nullopt once closed AND drained
    std::optional<T> pop() {
        std::optional<T> dropped; 
        {
            std::unique_lock lock(m_); 

            not_empty_.wait(lock, [&] { return size_ > 0 || closed_; });
            if(size_ == 0) return std::nullopt; 

            //constructed in place to avoid problemd with T without default constructor
            dropped.emplace(std::move(*buffer_[read_idx_])); 
            buffer_[read_idx_].reset(); 
            read_idx_ = (read_idx_ + 1) % capacity_; 
            size_--; 
        }
        not_full_.notify_one(); 
        return dropped;
    }

    
    // wakes everyone; no new pushes accepted after this
    void close() {
        {
            std::lock_guard lock(m_);
            closed_ = true; 
        }
        not_empty_.notify_all(); 
        not_full_.notify_all(); 
    }           
}; 

int main() {
    constexpr int P = 4, C = 4, N = 100'000;
    BlockingQueue<int> q(16);
    std::vector<long long> sums(C, 0);
    std::vector<std::thread> producers, consumers;

    for (int c = 0; c < C; ++c)
        consumers.emplace_back([&, c] { while (auto v = q.pop()) sums[c] += *v; });
    for (int p = 0; p < P; ++p)
        producers.emplace_back([&] { for (int i = 0; i < N; ++i) q.push(i); });

    for (auto& t : producers) t.join();
    q.close();
    for (auto& t : consumers) t.join();

    long long total = 0;
    for (auto s : sums) total += s;
    long long expected = 1LL * P * N * (N - 1) / 2;
    std::cout << total << (total == expected ? " OK\n" : " MISMATCH\n");
}