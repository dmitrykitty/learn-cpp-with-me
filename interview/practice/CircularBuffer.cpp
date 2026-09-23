#include <optional>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <iostream>

/*
 * V1 - vector as field, need to call default constructor for each T object during initialization
 * V2 - using allocator and allocator traits for row pointer
 *
 */

using size_type = std::size_t;


//Rule of 5.
//Manually defined destructor -> copy,move constr/assignment
template <typename T> 
class CircularBuffer {
    using Alloc = std::allocator<T>;
    using Traits = std::allocator_traits<Alloc>;

    T* buffer_ = nullptr;
    //head is index of first element
    size_type head_{0}; 
    //tail is next index after last element
    size_type tail_{0};
    size_type size_{0};  
    size_type capacity_{0};
    [[no_unique_address]]Alloc alloc_;

    //later we can change it into (cur + 1) & (cap - 1) <-- mask 
    size_type next(size_type cur) const {
        return (cur + 1) % capacity_;
    } 

public: 
    explicit CircularBuffer(size_type capacity){
        if(capacity == 0) {
            throw std::invalid_argument("capacity must be > 0"); 
        }
        buffer_ = Traits::allocate(alloc_, capacity);
        capacity_ = capacity; 
    }
    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;

    ~CircularBuffer() {
        for (size_type i = 0; i < size_; ++i) {
            Traits::destroy(alloc_, buffer_ + head_);
            head_ = next(head_);
        }
        Traits::deallocate(alloc_, buffer_, capacity_);
    }

    //tail moved
    bool push(const T& value) {
        if(full()) {
            return false; 
        }

        Traits::construct(alloc_, buffer_ + tail_, value);
        tail_ = next(tail_); 
        size_++;
        return true;
    }

    //head moved
    std::optional<T> pop() {
        if(empty()) {
            return std::nullopt; 
        }

        //head is moved forward
        T cur = buffer_[head_];
        Traits::destroy(alloc_, buffer_ + head_);
        head_ = next(head_); 
        size_--; 
        return cur; 
    }

    //precondition - buffer not empty
    const T& front() const {
        return buffer_[head_];
    }

    T& front() {
        return buffer_[head_];
    }

    bool empty() const {
        return size_ == 0; 
    }

    bool full() const {
        return size_ == capacity_; 
    }


    size_type size() const {
        return size_;
    }

    size_type capacity() const {
        return capacity_;
    }

    //------------------------------
    //for tests only!!!
    size_type head() const {
        return head_;
    }
    size_type tail() const {
        return tail_;
    }
};


//TESTS
int main() {
    static_assert(sizeof(CircularBuffer<int>) == 40);
    CircularBuffer<int> buff(4);
    assert(buff.capacity() == 4);
    buff.push(1);  
    buff.push(2); 
    buff.push(3); 
    assert(buff.head() == 0);
    assert(buff.tail() == 3);
    assert(buff.size() == 3);

    const std::optional<int> res = buff.pop();
    std::cout << res.value();
    assert(res.value() == 1);
    assert(buff.head() == 1); 
    assert(buff.size() == 2); 

    buff.push(4);
    buff.push(5); 
    assert(buff.head() == buff.tail()); 
    assert(buff.full()); 
    assert(!buff.push(6)); 

    assert(buff.pop().value() == 2);
    assert(buff.pop().value() == 3);
    assert(buff.pop().value() == 4);
    assert(buff.pop().value() == 5);
    assert(!buff.pop().has_value());
    assert(buff.empty());
}