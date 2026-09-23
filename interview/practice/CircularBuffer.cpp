#include <optional>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <utility>

/*
 * V1 - vector as field + head/tail/size, need to call default constructor for each T object during initialization
 * V2 - using allocator and allocator traits for raw storage management
 * V3 - support for move only objects, for example unique_ptr. Two separate overloaded function push(const T&) and push(T&&)
 * V4 - perfect forwarding support
 * V5 - move constructor and move assignment operator provided
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

    void swap(CircularBuffer& other) noexcept{
        std::swap(buffer_, other.buffer_);
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(alloc_, other.alloc_);
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

    CircularBuffer(CircularBuffer&& other) noexcept :
        buffer_(std::exchange(other.buffer_, nullptr)),
        head_(std::exchange(other.head_, 0)),
        tail_(std::exchange(other.tail_, 0)),
        size_(std::exchange(other.size_, 0)),
        capacity_(std::exchange(other.capacity_, 0)),
        alloc_(std::move(other.alloc_)) {}

    //move and copy (if it is not deleted) assignment operators
    CircularBuffer& operator=(CircularBuffer other) noexcept {
        swap(other);
        return *this;
    }

    ~CircularBuffer() {
        for (size_type i = 0; i < size_; ++i) {
            Traits::destroy(alloc_, buffer_ + head_);
            head_ = next(head_);
        }
        //we can assume that deallocate(alloc_, nullptr, 0) will not work
        if (buffer_ != nullptr) {
            Traits::deallocate(alloc_, buffer_, capacity_);
        }
    }

    //tail moved
    //for copyable objects
    bool push(const T& value) {
        if(full()) {
            return false; 
        }

        Traits::construct(alloc_, buffer_ + tail_, value); //copy
        tail_ = next(tail_); 
        size_++;
        return true;
    }

    //rvalue overload
    //T&& is rvalue reference, but it has name, name is expression-lvalue
    //so we need to cast value to rvalue
    bool push(T&& value) {
        if(full()) {
            return false;
        }

        Traits::construct(alloc_, buffer_ + tail_, std::move(value));
        tail_ = next(tail_);
        size_++;
        return true;
    }

    //we don't want each time to create temp object just to path it as param to push function by creating extra copy
    //so we introduce template function with Args of which object T is created ( T(arg1, arg2, ...))
    // std::forward preserves the original value category of each forwarded argument
    //perfect forwarding / universal references
    //What needed:
    //  1) template function
    //  2) template param has &&
    //  3) use of std::forward to move forward true type of the reference
    template<typename... Args>
    //Args... - pack of types
    //args... - pack of objects
    //for push() T&& comes directly from class, so it just type
    //here Args&& is template param, so true type of Args is deduced
    //and thanks to reference collapsing we get true type & or &&
    bool emplace(Args&&... args) {
        if(full()) {
            return false;
        }
        //use std::forward for each pair <ArgX, argx>
        Traits::construct(alloc_, buffer_ + tail_, std::forward<Args>(args)...);
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
        T cur = std::move(buffer_[head_]); //completely new object
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

    T* buffer() const {
        return buffer_;
    }
};


struct Order {
    int id;
    std::string symbol;
    std::unique_ptr<int> quantity;

    Order(int id, std::string symbol, std::unique_ptr<int> quantity)
        : id(id),
          symbol(std::move(symbol)),
          quantity(std::move(quantity)) {}
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

    CircularBuffer<std::unique_ptr<int>> b(50);
    auto p = std::make_unique<int>(5); //p -lvalue
    b.push(std::move(p)); //no copy, so make p rvalue
    assert(b.size() == 1);
    assert(*b.front() == 5);
    assert(p == nullptr);

    const auto result = b.pop();
    assert(result.has_value());
    assert(*result.value() == 5);

    CircularBuffer<Order> ordersA(5);

    std::string symbol = "NVDA";
    auto qty1 = std::make_unique<int>(100);
    auto qty2 = std::make_unique<int>(100);

    ordersA.emplace(
        42,
        symbol,             // lvalue -> copy string
        std::move(qty1)      // rvalue -> move unique_ptr
    );
    ordersA.emplace(
        43,
        symbol,             // lvalue -> copy string
        std::move(qty2)      // rvalue -> move unique_ptr
    );

    CircularBuffer<Order> ordersB(std::move(ordersA));
    assert(qty1 == nullptr && qty2 == nullptr);
    assert(ordersB.size() == 2);
    assert(ordersB.front().id == 42);
    assert(ordersA.empty());
    assert(ordersA.buffer() == nullptr);






}