#include <iostream>

// 4.2. Visibility and name conflicts 

struct Base {
protected:
    void df() {
        std::cout << "df";
    }
public:
    int x = 1; 
    void f() {
        std::cout << 1; 
    }

    void h(int, int) {
        std::cout << 1;
    }
}; 

struct Derived: Base {
private:
    int x = 2; 
public: 
    //now df is public  
    using Base::df; 

    void f() {
        std::cout << 2;
    }

    void h(int) {
        std::cout << 2;
    }
}; 


int main() {
    Derived d; 
    d.f(); //2

    //trying get public x from base is impossible. Firsly selected more preferred option:
    //  - for us it is x from Derived, because we are trying to call it from Derived and it's much more specified and we even do not check names from
    //  - only now access modifier checked and we get CE
    //d.x

    d.h(1); //2
    //the same reason. We even do noy check h() from Base because h() from Derived more specified
    //CE - even if function with two arguments exists in Base - we do not check it - h from Derived is preferred
    //d.h(1, 1); 
    //but we can call function from Base
    d.Base::h(1, 1); //1

    //expanding use of df by making it public 
    d.df();
}