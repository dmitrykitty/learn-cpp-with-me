#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

class IntBuffer {
    std::size_t size_;
    int* data_;

    void swap(IntBuffer& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

public:
    explicit IntBuffer(std::size_t sz)
        : size_(sz)
        , data_(size_ > 0 ? new int[size_] : nullptr) {}
    
    IntBuffer(const IntBuffer& other)
            : size_(other.size_)
            , data_(size_ > 0 ? new int[size_] : nullptr) {
        if(size_ > 0) {
            std::copy(other.data_, other.data_ + size_, data_);
        }
        
    }
    IntBuffer& operator=(const IntBuffer& other) {
        if(this != &other) {
            IntBuffer tmp = other;
            swap(tmp);
        }
        return *this;
    }

    IntBuffer(IntBuffer&& other) noexcept: size_(other.size_), data_(other.data_) {
        other.size_ = 0; 
        other.data_ = nullptr;
    }
    IntBuffer& operator=(IntBuffer&& other) noexcept {
        if(this != &other) {
            delete[] data_; 
            size_ = other.size_; 
            data_ = other.data_; 
            other.size_ = 0; 
            other.data_ = nullptr; 
        }
        return *this;
    }

    ~IntBuffer() {
        delete[] data_;
    }

    std::size_t size() const {
        return size_;
    }
    int& operator[](std::size_t index) {
        if(index >= size_) {
            throw std::out_of_range("index out of range");
        }
        return data_[index];
    }
    const int& operator[](std::size_t index) const {
        if(index >= size_) {
            throw std::out_of_range("index out of range");
        }
        return data_[index];
    }
};