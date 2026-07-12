#ifndef STRING_H
#define STRING_H

#include <cstddef>
#include <iostream>

class String {
    std::size_t size_ = 0; 
    std::size_t capacity_ = 0; 
    char* data_ = nullptr;
public: 
    String();
    String(const char*); 
    String(std::size_t, char);
    explicit String(char);
    String(const String&); 
    String(std::initializer_list<char>);
    ~String();

    String& operator=(String);
    String& operator+=(const String&); 
    const char& operator[](std::size_t) const;
    char& operator[](std::size_t);
    std::strong_ordering operator<=>(const String&) const;
    bool operator==(const String&) const;

    void push_back(char c); 
    void reserve(std::size_t); 
    void clear();
    void pop_back(); 
    void shrink_to_fit(); 
    const char& front() const;
    char& front(); 
    const char& back() const; 
    char& back();  

    int find(const String&) const;
    int rfind(const String&) const; 
    String substr(std::size_t, std::size_t) const;


    std::size_t size() const {
      return size_; 
    }

    const char* c_str() const {
      return data_;
    }

    bool empty() const {
      return size_ == 0; 
    }

private:
    void swap(String&) noexcept;
};

String operator+(String s1, const String& s2); 
std::ostream& operator<<(std::ostream&, const String&);
std::istream& operator>>(std::istream&, String&);

#endif

