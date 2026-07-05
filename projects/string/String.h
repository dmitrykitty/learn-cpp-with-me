#ifndef STRING_H
#define STRING_H

#include <cstddef>
#include <iostream>

class String {
    size_t size_ = 0; 
    size_t capacity_ = 0; 
    char* data_ = nullptr;
public: 
    String();
    String(const char*); 
    String(size_t, char);
    explicit String(char);
    String(const String&); 
    String(std::initializer_list<char>);
    ~String();
};

#endif

