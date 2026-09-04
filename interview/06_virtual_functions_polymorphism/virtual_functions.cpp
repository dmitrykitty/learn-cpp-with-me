#include <iostream>

/*
Type is polymorphic when it has at least one virtual function 
final tells the compiler that a function cannot be overridden further, or that a class cannot be derived from. At call sites where the static type gives enough information, this may allow devirtualization: replacing an indirect virtual call with a direct call. That removes the vtable lookup and indirect branch, but the more important benefit can be enabling inlining and further optimizations. final does not guarantee an optimization, and compilers can often devirtualize without it when they can otherwise prove the dynamic type. It also generally doesn't remove the vptr or vtable from a polymorphic class.
*/
namespace EX1 {
    namespace NonVirtual {
        struct Base {
            void f() { std::cout << "Base"; }
        };

        struct Derived: Base {
            void f() { std::cout << "Derived"; }
        };
    }

    namespace Virtual {
        struct Base {
            virtual void f() { std::cout << "Base"; }
            virtual void h() {}
        };

        struct Derived: Base {
            //this function is also virtual because it repeats signature of f from Base class
            //key word override does not make function virtual it only informs if signature mismatch happened
            void f() override {std::cout << "Derived";}

            //this function can't be overriden in down classes
            //why it is type of optimization? 
            //fina also can be class
            void h() final {}
        };
    }


    void virtual_non_virtual() {
        NonVirtual::Derived d; 
        d.f(); //Derived
        NonVirtual::Base& b = d; 
        b.f(); //Base
        std::cout << '\n';
        Virtual::Derived dv; 
        dv.f(); //Derived
        Virtual::Base& bv = dv; 
        bv.f(); //Derived
    }
}

namespace EX2 {
    struct Base {
        virtual void f() {}
        virtual ~Base() {
            std::cout << "~Base";
        }
    };

    struct Derived: Base {
        int* a = new int(1);
        ~Derived() {
            delete a; 
            std::cout << "~Derived";
        }
    };

    //delete calls destructor so to call destructor from Derived class having pointer to Base 
    //we need to make destrucxtor virtual so type of the object will be recognized during runtime 
    void virtual_destructor() {
        Base* b = new Derived();
        delete b;
    }
}

namespace EX3 {
    struct Granny {
        virtual void f() const {
            std::cout << "Granny";
        }
    };

    struct Mom: Granny {
    private:
    // Access control is checked for the function selected by compile-time name lookup, while overriding/final-overrider selection is independent of access control.
        void f() const override {
            std::cout << "Mom";
        }
    };

    struct Son: Mom {
        void f() const final {
            std::cout << "Son";
        }
    };

    void virtual_privacy() {
        Mom m; 
        Granny& g = m; 
        g.f(); //mom
        // m.f(); but here we have object of mom, so function call selected during compile time
    }

}

namespace EX4 {
    struct Mom {
        virtual void f() { std::cout << "Mom"; }
        virtual void h() { std::cout << "Mom"; }
    };

    struct Dad {
        void f() { std::cout << "Dad"; }
        virtual void h() { std::cout << "Dad"; }
    };

    struct Son: Mom, Dad {
        //this function is virtual but it continue hierarchy of mom f function, not dad
        void f() { std::cout << "Son"; }
        void h() override { std::cout << "Son"; }
    };

    /*
    Mom   Dad
    \     /
    \    /
     Son
    */

    void two_parents_hierachy() {
        Son s;
        s.f(); //Son
        Mom& m = s;
        m.f(); //Son
        m.h(); //Son
        Dad& d = s;
        d.f(); //Dad
        d.h(); //Son
        
    }
}

namespace EX5 {
struct Base {
    virtual void f(int x = 10) {
        std::cout << x << "Base";
    }
};

struct Derived : Base {
    void f(int x = 20) override {
        std::cout << x << "Derived";
    }
};



void virtual_f_with_default_args() {
    Derived d;
    Base& b = d;
    b.f(); //10Derived
}
}

int main() {
    EX1::virtual_non_virtual();
    std::cout << "\n-------------------------------------\n";
    EX2::virtual_destructor();
    std::cout << "\n-------------------------------------\n";
    EX3::virtual_privacy();    
    std::cout << "\n-------------------------------------\n";
    EX4::two_parents_hierachy();
    std::cout << "\n-------------------------------------\n";
    EX5::virtual_f_with_default_args();
    std::cout << "\n-------------------------------------\n";
    std::cout << "\n-------------------------------------\n";
    std::cout << "\n-------------------------------------\n";
}