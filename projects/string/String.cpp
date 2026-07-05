#include "String.h"
#include <cstring>
#include <cctype>
#include <algorithm>

//-----------------------PUBLIC--------------------------

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

void String::reserve(std::size_t new_cap) {
    if(capacity_ >= new_cap) {
        return;
    }
    char* new_data = new char[new_cap + 1]; 
    memcpy(new_data, data_, size_ + 1); 
    delete[] data_; 
    data_ = new_data; 
    capacity_ = new_cap; 
}

void String::clear() {
    size_ = 0;
    data_[0] = '\0';
}

//Copy and Swap idiom
String& String::operator=(String other_copy) {
    swap(other_copy);
    return *this;
}

String& String::operator+=(const String& other) {
    std::size_t new_size = size_ + other.size_; 
    reserve(std::max(new_size, capacity_ * 2));
    memcpy(data_ + size_, other.data_, other.size_);
    size_ = new_size; 
    data_[size_] = '\0';
    return *this; 
}

const char& String::operator[](std::size_t idx) const {
    return data_[idx];
}
char& String::operator[](std::size_t idx) {
    return data_[idx];
}

void String::push_back(char c) {
    if(capacity_ == size_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    data_[size_++] = c; 
    data_[size_] = '\0';
} 

//-------------------------PRIVATE-------------------------------

void String::swap(String& other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(data_, other.data_);
}

//-----------------------FUNCTIONS-------------------------------

String operator+(String lhs, const String& rhs) {
    lhs += rhs; 
    return lhs;
}

std::ostream& operator<<(std::ostream& out, const String& s) {
    for(std::size_t i = 0; i < s.size(); ++i) {
        out << s[i];
    }
    return out;
}

std::istream& operator>>(std::istream& in, String& s) {
    s.clear();
    int c = in.get();
    while(c != EOF && isspace(static_cast<unsigned char>(c))) {
        c = in.get(); 
    }

    while(c != EOF && !isspace(static_cast<unsigned char>(c))) {
        s.push_back(c); 
        c = in.get(); 
    }
    return in;
}