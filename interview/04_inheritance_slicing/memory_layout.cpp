#include <iostream>

namespace EX1 {
    struct A {
        int a;
    };

    struct B: A {
        double b;
    };

    /*
    B
    fields A | fields B
    */

    void layout_example() {
        B b; 
        std::cout << alignof(B); //if b - char 4, if double - 8
        std::cout << sizeof(B); // if b - char 8, if double - 16
    }
}

namespace EX2 {
    class A {
    };

    //EBO does not apply because A is a member, size 1 byte, so size B is 8
    class B1: A {
        int y;
        A a; 
        
    };

    //EBO does not apply, B1 size if 8 bytes, + 4 for int + padding = 16
    class B2: A {
        B1 b1;
        int x; 
    };


    //EBo applies, size = 4
    class C : A {
        int x; 
    };

    void empty_base_optimization() {
        std::cout << sizeof(A) << sizeof(B1) << sizeof(B2) << sizeof(C) << '\n';
    }
}

/*
Member initialized with 1
Base initialized with 2
Member initialized with 3
Derived initialized with 4
Derived was deleted with 4
Member was deleted with 3
Base was deleted with 2
Member was deleted with 1
*/
namespace EX3 {
    struct Member {
        int m; 
        Member(int m): m(m) {
            std::cout << "Member initialized with " << m << "\n"; 
        }

        ~Member() {
            std::cout << "Member was deleted with " << m << "\n"; 
        }
    };
    struct Base {
        Member mb{1};
        int x; 
        Base(int x): x(x) {
            std::cout << "Base initialized with " << x << "\n"; 
        }

        ~Base() {
            std::cout << "Base was deleted with " << x << "\n"; 
        }
    };

    struct Derived: Base {
        Member md{3};
        int y; 
        Derived(int x, int y): Base(x), y(y) {
            std::cout << "Derived initialized with " << y << "\n";
        }

        ~Derived() {
            std::cout << "Derived was deleted with " << y << "\n"; 
        }
    };

    void constructing_order() {
        Derived d(2, 4);
    }
}

namespace EX4 {
    struct Base {
        int x; 
        //10 different constructors 
        Base(int x): x(x) {}
    };

    struct Derived: Base {
        //no extra fields, but we dont wanty ro write all the same 10 constructors from Base 
        using Base::Base; 
    };


    void using_base_constructors() {
        Derived d = 10; 
        std::cout << d.Base::x << d.x << '\n';
    }
}

namespace EX5 {
    struct Base {
        int x; 
        Base(int x): x(x) {
            std::cout << "base"<< x << '\n';
        }
        Base(const Base& other): x(other.x) {
            std::cout << "copyBase\n";
        }
    };

    struct Derived: Base {
        int y = 0;
        using Base::Base; 

        Derived(int y): Base(0), y(y) {
            std::cout << "derived\n";
        }
    };

    void implicit_copy_cosntyructor() {
        Derived d(1); //ok, was used constructor from >> derived
        Derived d2(d); //implicitly was generated copy cosntructor for Derived
        //if copy constructor for derived explicitly deleted - CE, because we cant create Derived from Base
        std::cout << d.y << d2.y << '\n'; //thats why d2.y = 1
    }

    //So copy construcotrs, move constructors are not inherited! 
}

int main() {
    EX1::layout_example(); 
    EX2::empty_base_optimization();
    EX3::constructing_order();
    EX4::using_base_constructors();
    EX5::implicit_copy_cosntyructor();
}