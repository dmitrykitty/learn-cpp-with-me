#include <iostream>

namespace EX1 {
struct Base {
    int x = 0;
    virtual void f() {}
    virtual ~Base() = default; 
}; 

struct Derived: Base {
    int y; 
    void f() override {}
};

void dynamic_cast_example() {
    Derived d; 
    Base* b = &d; 
    //SOURCE type should be polymorphic 
    //its quite expensive operation and need few pointer calls (+ memory overhead) 
    //works during runtime
    //so static cast gives us only UB
    //thanks to information about type RTTI dynamic cast works 
    Derived* dd = dynamic_cast<Derived*>(b); //nullptr or std::bad_cast if dynamic type is not Derived

    //we can make cast to void* from any polymorphic type
    void* vdd = dynamic_cast<void*>(b);


    // UPCAST Derived* -> Base*
    //
    // implicit       : OK if Base is accessible and unambiguous
    // static_cast    : OK, performs correct pointer adjustment
    // dynamic_cast   : OK, performs correct pointer adjustment
    // reinterpret_cast: DO NOT use; does not perform inheritance-aware adjustment


    // DOWNCAST Base* -> Derived*
    //
    // dynamic_cast   : checked at runtime; Derived* or nullptr
    // static_cast    : unchecked; OK only if object is really a Derived;
    //                  otherwise undefined behavior
    // reinterpret_cast: DO NOT use; no inheritance-aware adjustment
    Derived* d1 = dynamic_cast<Derived*>(b); // checked
    // d1 == nullptr if b doesn't point to appropriate Derived object

    Derived* d2 = static_cast<Derived*>(b);  // unchecked
    // valid if b really points to Base subobject of Derived
    // UB otherwise

    Derived* d3 = reinterpret_cast<Derived*>(b); // wrong tool
}

namespace EX3 {
struct Mom {
    virtual ~Mom() = default;
};

struct Dad {
    virtual ~Dad() = default;
};

struct Child : Mom, Dad {};    

void cross_cast_example() {
    Child c;
    Mom* mom = &c;
    Dad* d1 = dynamic_cast<Dad*>(mom); // OK: runtime-checked cross-cast

    Dad* d2 = static_cast<Dad*>(mom);  // compile error

    Dad* d3 = reinterpret_cast<Dad*>(mom); // compiles, but wrong mechanism
}
}



namespace EX2 {
struct Base {
    int x = 0;
    virtual void f() {}
    virtual ~Base() = default; 
}; 

struct Derived: Base {
    int y; 
    void f() override {}
};

void type_id_example() {
    Derived d; 
    Base& b = d; 
    Base* pb = &d;

    //typeid - operator -> typeinfo
    //have information about dynamic name of the object
    //works also for not polymorphic types
    //for pointer it doesnt show dynamic type but static type of the pointer 
    std::cout << typeid(d).name() << typeid(b).name() << typeid(int).name() << '\n'; //N3EX27DerivedE N3EX27DerivedE i
    std::cout << typeid(d).name() << typeid(pb).name(); //N3EX27DerivedE PN3EX24BaseE

    //we can use c++filt -t to decode types 
    //N3EX27DerivedE -> EX2::Derived
    //PN3EX24BaseE -> EX2::Base*
    //i -> int

}
}

int main() {
    EX2::type_id_example();

}