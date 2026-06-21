#include <iostream>
#include <vector>
#include <cstring>

//3.4. Assignment operator ad rule of three 
//If any of copy cosntructor, copy assigment operator, destructor is declared 
//all three should be declared 

class String {
    char* arr = nullptr; 
    size_t sz = 0; 
    size_t cap = 0; 

    String(size_t n): arr(new char[n + 1]), sz(n), cap(n + 1) {
        arr[sz] = '\0';
    }
public:
    String() = default; 
    String(size_t n, char c): String(n) {
        memset(arr, c, sz); 

        std::cout << "String(sz, ch)\n";
    }

    String(std::initializer_list<char> list): String(list.size()) {
        std::copy(list.begin(), list.end(), arr);
        std::cout << "String(init_list)\n";
    }

    String(const String& other): String(other.sz) {
        memcpy(arr, other.arr, sz);

    }

    String& operator=(const String& other) {
        if(this == &other) {
            return *this; 
        }

        delete[] arr; 
        sz = other.sz; 
        cap = other.cap; 
        arr = new char[cap]; 
        memcpy(arr, other.arr, cap); 
        return *this; 
    }

    //COPY AND SWAP IDIOM
    void swap(String& other) {
        std::swap(sz, other.sz); 
        std::swap(cap, other.cap);
        std::swap(arr, other.arr);
    }

    #if 0
    //we make copy of other 
    //and swap with this all fileds 
    //after leaving operator= scope copy string with our old fields 
    //will be destroyed by destructor calling 

    //so for what make copy if we can get copy as argument 
    //and simply swap fields with copy 
    String& operator=(String other){
        swap(other); 
        return *this; 

    }
    #endif
    ~String() {
        delete[] arr; 
    }
};


int main() {

}