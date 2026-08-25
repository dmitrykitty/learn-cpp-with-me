#include <iostream>
#include <cassert>

namespace EX1 {
struct A {
    int x = 0; 
};

struct B {
    int y = 1; 
};

struct C: A, B {
    int z = 2;
}; 

// | A.x | B.y | C.z |


void size_of_objects_shifts() {
    std::cout << sizeof(C) << '\n'; //4 * 3 = 12 

    C c; //...35C
    A* a = static_cast<A*>(&c); //...35C
    B* b = static_cast<B*>(&c); //...360 So shift 4 bytes 

    std::cout << a << " " << b << " " << &c << '\n';
    //Example of multiple diamong inheritance in STL - streams 

    B& bb = static_cast<B&>(c);
    // invalid type conversion - NO SIDE CAST A -> B OR A <- B
    // A& aa = static_cast<A&>(bb);
}
}

/*
           D
        /     \
       B       C
       |       |
      A₁      A₂

      D
+------------------+
| B                |
|   +------------+ |
|   | A #1       | |
|   +------------+ |
+------------------+
| C                |
|   +------------+ |
|   | A #2       | |
|   +------------+ |
+------------------+
| D members        |
+------------------+
*/
namespace EX2 {
    struct A {
        int x;
        void print_x() { std::cout << x << '\n'; }
        static void print_static() { std::cout << "static method call\n"; }
    };

    struct B : A {
        void f(int){}
    };
    struct C : A {
        void f(int, int) {}
    };

    struct D : B, C {};    

    void diamond_inheritance() {
        D d; 
        d.B::x = 1; 
        d.C::x = 2; 
        //Two different A-objects
        d.B::print_x();
        d.C::print_x();  

        std::cout << sizeof(D) << '\n'; //8 because of double A 

        //ambigious - no unique A prsented
        //A* a = static_cast<A*>(&d);
        //A* a - static_cast<B::A*>(&d); //the same

        B* b = static_cast<B*>(&d);
        A* ab = static_cast<A*>(b);
        
        C* c = static_cast<C*>(&d);
        A* ac = static_cast<A*>(c);
        std::cout << (ac != ab) << '\n'; //1

        // OK: both inheritance paths find the same static member declaration.
        // A static member has no `this`, so no particular A subobject
        // needs to be selected.
        d.print_static();

        //=======================================
        // interesting that it is not overload set but ambigious call 
        // Name lookup is ambiguous before overload resolution even begins.
        // d.f(1, 2); 
    }
}

//Example of Warning inaccessible base 
namespace EX3 {
    struct A {
        int x; 
    };

    struct B: A {

    };

    struct C: A, B {

    };

    void triangular_inheritance() {
        std::cout << sizeof(C) << '\n'; //8

        //we can get access to the A through B
        C c; 
        c.B::x = 1; 
        std::cout << c.B::x << '\n';

        //but here call is ambigious
        // c.x = 2; 
    }
}

namespace EX4 {
    struct A {
        void f() {
            std::cout << 1;
        }
    };

    struct B { 
        void f() {
            std::cout << 2;
        }
    }; 

    struct C: A, B {
    };

    void ff(int x){ std::cout << x; }

    void pointer_to_methods() {
        // Free functions and static member functions can undergo
        // function-to-pointer conversion, so '&' is optional.
        void (B::*pfb)() = &B::f; //pointer to B::f
        void (A::*pfa)() = &A::f; //pointer to A::f
        void (*pff)(int) = ff; //pointer to global function ff
        C c; 
        std::cout << sizeof(pfb) << " " << sizeof(pfa) << " " << sizeof(pff) << '\n'; //16 16 8

        // A pointer-to-member cannot be called by itself.
        // It has to be combined with an object (.*) or object pointer (->*).
        //
        // Since C derives from B and A, these expressions select the appropriate
        // base subobject of c and invoke the member function with that subobject
        // as `this`.
        (c.*pfb)(); 
        (c.*pfa)(); 
        ff(3);

        /*
        * Interesting observation:
        *
        * On this platform, a normal function pointer is 8 bytes, while a
        * pointer-to-member function is 16 bytes.
        *
        * This size is NOT guaranteed by C++. It depends on the compiler and ABI.
        *
        * A normal function pointer usually only needs enough information to
        * identify the function to call.
        *
        * A non-static member function is more complicated because the call also
        * needs a valid `this` pointer. With inheritance, and especially multiple
        * inheritance, the base subobject on which the function operates may not
        * start at the same address as the complete derived object.
        *
        * Therefore an implementation may need additional information for
        * adjusting `this` before making the call.
        *
        * Under a common 64-bit Itanium ABI representation, a pointer-to-member
        * function is conceptually two machine words:
        *
        *     +----------------------+
        *     | function information |
        *     +----------------------+
        *     | this adjustment      |
        *     +----------------------+
        *
        * which explains the observed 16 bytes = 8 + 8.
        *
        * However, we should NOT interpret this as simply storing
        * "the offset of B inside C". A B::* does not know that it will later be
        * used with C at all. It may be used with B itself or with many different
        * classes derived from B.
        *
        * The exact representation is ABI-specific, and virtual member functions
        * can make the representation even more interesting because the stored
        * information may describe how to locate the final function rather than
        * simply containing a raw function address.
        */

        /*
        It is very unlikely that a pointer-to-member function stores an offset relative to some hypothetical future derived object, as it may sometimes sound in explanations. At the moment the pointer is created, we do not know through an object of which derived class it may eventually be called, so such an offset cannot be stored.

        As I understand it, a pointer-to-member function representation may indeed contain a this adjustment, but that adjustment is related to the class type of the member pointer itself.

        For example, with:

        void (Dad::*pf)() = &Dad::f;

        the adjustment will typically be 0, because the function already expects a Dad* as its implicit this.

        But if we convert a pointer-to-member of the base class to a pointer-to-member of the derived class, for example:

        void (Son::*pcf)() = static_cast<void (Son::*)()>(&Dad::f);

        then the relationship between Son and Dad is known. If the Dad subobject starts, for example, at offset +4 inside Son, that adjustment can be encoded in the member-function pointer.

        Then, when the function is called through a Son object, the this pointer is first adjusted so that it points to the Dad subobject, and only then Dad::f is called with the correct implicit this.
        */
    }
}
    


int main() {
    EX1::size_of_objects_shifts();
    std::cout << "=====================\n";
    EX2::diamond_inheritance();
    std::cout << "=====================\n";
    EX3::triangular_inheritance();
    std::cout << "=====================\n";
    EX4::pointer_to_methods();
}