#include <iostream>

// 3.7. Pointers to members

struct S {
    int x;
    double y;

    void f(int z) {
        std::cout << x + z;
    }
};

int main() {
    // Pointer to a non-static data member of S.
    // It does not point to a complete object and cannot be dereferenced alone.
    int S::* p = &S::x;

    // On simple standard-layout classes, it is often implemented similarly
    // to an offset from the beginning of an S object.
    // However, the C++ standard does not guarantee that it is literally
    // a byte offset. Inheritance can make the representation more complex.

    S s{1, 2.0};

    // .* applies a pointer-to-member to an object.
    std::cout << s.*p; // 1

    S* ps = &s;

    // ->* applies a pointer-to-member to a pointer to an object.
    std::cout << ps->*p; // 1

    // Parentheses are required because of operator precedence.
    (s.*p) = 10;       // modifies s.x
    std::cout << s.x;  // 10

    // Pointer to a non-static member function:
    // function belongs to S, takes int, and returns void.
    void (S::*pf)(int) = &S::f;

    // A member-function pointer also needs an object because the call
    // requires an implicit `this` pointer.
    (s.*pf)(5);   // prints 15
    (ps->*pf)(7); // prints 17

    // A pointer to member can be null.
    int S::* null_member = nullptr;

    if (null_member != nullptr) {
        std::cout << s.*null_member;
    }

    // Pointer-to-member is different from a normal pointer:
    //
    // int*          -> address of a concrete int object
    // int S::*      -> describes which int member to select from an S object
    //
    // void (*)(int)       -> normal function pointer
    // void (S::*)(int)    -> member-function pointer requiring an S object

    // Member-function pointers may be larger than ordinary function pointers.
    // With multiple or virtual inheritance they may contain:
    // - function address,
    // - adjustment for the hidden `this` pointer,
    // - additional ABI-specific information.
}