#include <iostream>
// Dependent names

/*
Two-phase lookup in templates

A template is processed conceptually in two stages.

Phase 1 — template definition time:
- the compiler parses the template before concrete template arguments are known;
- syntax is checked;
- non-dependent names are looked up immediately;
- declarations found by ordinary lookup for non-dependent names are fixed at this point;
- dependent constructs cannot always be fully checked yet.

Phase 2 — template instantiation time:
- concrete template arguments are known;
- dependent types, members and expressions can now be resolved;
- additional semantic checks depending on those arguments are performed;
- for dependent unqualified function calls, ADL may add functions from
  namespaces/classes associated with the instantiated argument types.

Important consequence:
A function declared after the template definition is NOT automatically visible
just because it exists before the template is instantiated.

For dependent unqualified function calls:
- ordinary lookup is based on what was visible at template definition time;
- at instantiation time ADL may add additional candidates.

Therefore:
    lookup at instantiation != "run normal lookup again from scratch".

This distinction explains many seemingly strange template behaviors.
*/

#include <array>

namespace EX1 {
template <typename T>
struct S {
    using A = int; 
    //static const int A = 5; ups:) 
};

template<typename T>
void f() {
    //S<T>::A* x; what the problem - in can be parsed as static variable A from S<T> * x and x is undeclared
    //so to inform compiler that we want to use A as a type 
    typename S<T>::A* x; //and now after instantiation it looks like int* x if T == int 
}
}

namespace EX2 {
template <typename T>
struct S {
    template <int N>
    using A = std::array<int, N>; 
}; 

template <typename T>
void f() {
    //S<T>::A<5> x; it parsed like static S::A < 5 > x and x wasn't declared in this scope xDDD
    //typename S<T>::A<5> x;  not enough because using use template inside so we want A threated as name of template 
    typename S<T>::template A<5> x; 
}
}

namespace EX3 {
template <typename T>
struct S {
    template<int N>
    void foo(int) {}
};

template<typename T> 
void bar(int x, int y) {
    S<T> s; 
    s.foo<5>(x + y); //here the problem that could exists such specialization that has member foo inside. So compiler don't know is it correct or wrong 
    //so to highligh that we have template function foo 
    s.template foo<5>(x + y); 
}
}

namespace EX4 {
template <typename T>
struct S {
    int x = 0;
};  

template<>
struct S<double> {
};

//S<T> is a dependent base class.
//so we need to explicitly select from where this x appears 
template<typename T>
struct SS: S<T> {
    void f() {
        this->x++; 
    }
};
}

// f(0) is a non-dependent call.
// Ordinary lookup happens at template definition time,
// so only f(int) belongs to the overload set.
// The later f(double) is not considered.
namespace EX5 {
void f(int) {
    std::cout << 1;
}

template<typename T>
void call(T x) {
    f(0); //// non-dependent call
}

//f is invisible for template 
//but it is obvious because for normal functions the same rule works 
void f(double) {
    std::cout << 2;
}

void test() {
    call(3.14);
}
}

/*
f(x) is a dependent call because the type of x depends on T.

At template definition time, ordinary lookup finds only f(long).

At instantiation time, ordinary lookup is NOT performed again from scratch.
Only ADL may add additional candidates.

For T = int, int is a built-in type and has no associated namespace,
so ADL finds nothing.

Therefore f(int), declared after the template definition, is not considered.
The only candidate is f(long), so the call prints 1.
*/
namespace EX6 {

void f(long) {
    std::cout << 1;
}

template<typename T>
void call(T x) {
    f(x);
}
// definition time:
// ordinary lookup finds f(long)

// instantiation time:
// ordinary lookup is NOT redone from scratch
 
//but ADL may add additional candidates
//because int is built-in type nothing will be added
void f(int) {
    std::cout << 2;
}

void test() {
    call(10);
}
}

int main() {
    EX5::test();//1
    EX6::test();//1
}