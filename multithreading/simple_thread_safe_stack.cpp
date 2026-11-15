#include <bits/stdc++.h>
#include <mutex>
#include <optional>

template <class T>
class ThreadSafeStack {
    std::vector<T> data_; 
    mutable std::mutex m_; 
public:
    ThreadSafeStack(ThreadSafeStack& other) {
        std::lock_guard lock(other.data_); // lock the SOURCE while reading it
        data_ = other.data_; 
    }

    void push(T value) {
        std::lock_guard lock(m_); 
        data_.push_back(std::move(value)); 
    }
    std::optional<T> try_pop() {
        std::lock_guard lock(m_); 
        if(!data_.empty()) {
            T old = std::move(data_.back()); 
            data_.pop_back(); 
            return old; 
        }
        return std::nullopt;
    }

    std::size_t size() const {
        std::lock_guard lock(m_); 
        return data_.size(); 
    }
};