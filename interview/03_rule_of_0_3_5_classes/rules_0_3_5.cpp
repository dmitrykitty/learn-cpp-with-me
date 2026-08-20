#include <iostream> 
#include <memory>

class A_0 {
    std::string name;
    
public:
    //default constructor will not be generated
    //note, if we can unique_ptr - no copy cosntructor and assignmen will be generated
    A_0(std::string nm): name(nm) {
        std::cout << "param constructor called\n"; 
    }
};

void rule_0() {
    A_0 a = A_0(std::string("name"));
    A_0 c(a); //copy constructor 
    c = a; //copy assignement 
    A_0 d(std::move(c)); //move constructor
    d = std::move(a); //move assingment
}

class B_3 {
    std::string name; 
    int sz;
    int* data = nullptr;
public:
    B_3(std::string nm, int size, int* dt): name(nm), sz(size > 0 ? size : 0), data(new int[sz]) {
        std::copy(dt, dt + sz, data); 
        std::cout << "param constructor called\n"; 
    } 
    
    B_3(const B_3& o): name(o.name),sz(o.sz > 0 ? o.sz : 0), data(new int[sz]) {
        std::copy(o.data, o.data + sz, data); 
        std::cout << "copy cosntructor called\n";
    }
    

    B_3& operator=(const B_3& o) {
        if(this == &o) {
            return *this;
        }
        delete[] data; 
        name = o.name; 
        sz = o.sz; 
        data = new int[sz];
        std::copy(o.data, o.data + sz, data); 
        std::cout << "copy assignment called\n";
        return *this;
    }

    //copy and swap idiom
    B_3& operator=(B_3 o) {
        swap(*this, o);
        return *this;
    }

    void swap(B_3& ts, B_3& o) {
        std::swap(ts.name, o.name);
        std::swap(ts.sz, o.sz);
        std::swap(ts.data, o.data);
    }

    ~B_3() {
        delete[] data;
    }
};


void rule_3() {
    B_3 b1(std::string("name"), 3, new int[]{1, 2, 3});
    B_3 b2(b1);
    b1 = b2; 
    B_3 b4(std::move(b2)); //want be generated, copy called bind from B&& -> const B&
    b4 = std::move(b1); //want be generated, copy called
}

//if no explicitly defined constructors and only destructor -> default, copy, copy assignment will be generated bit no move operations
int main() {
    rule_0(); 
    std::cout << "==========================\n";
    rule_3();
}