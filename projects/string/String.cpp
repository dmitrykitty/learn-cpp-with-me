#include "String.h"
#include <cstring>
#include <algorithm>

String::String(): size_(0), capacity_(0), data_(new char[1]{'\0'}) {}

String::String(const char* arr): size_(strlen(arr)), capacity_(size_), data_(new char[capacity_ + 1]) {
    if(arr == nullptr) {
        data_ = new char[1]{'\0'}; 
        return;
    }
    size_ = strlen(arr);
    capacity_ = size_; 
    data_ = new char[capacity_ + 1]; 
    data_[size_] = '\0'; 
    memcpy(data_, arr, size_);
}

String::String(size_t n, char c): size_(n), capacity_(size_), data_(new char[capacity_ + 1]) {
    data_[size_] = '\0'; 
    memset(data_, c, size_); 
}

String::String(char c): String(1, c) {}

String::String(const String& src): size_(src.size_), capacity_(src.capacity_), data_(new char[capacity_ + 1]) {
    data_[size_] = '\0';
    memcpy(data_, src.data_, size_);
}

String::String(std::initializer_list<char> lst): size_(lst.size()), capacity_(size_), data_(new char[capacity_ + 1]) {
    data_[size_] = '\0';
    std::copy(lst.begin(), lst.end(), data_); 
}

String::~String() {
    delete[] data_; 
}