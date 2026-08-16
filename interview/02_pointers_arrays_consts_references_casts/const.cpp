#include <iostream>

//pure fucntion without side effects
constexpr double square(double val) {
    return val * val;
}

//constexpr mean that it should be initialized with constant expression
//so we can say that it's const objects whitch value known during compilation

constexpr double twosquare = square(2); //calculated during compiletime (function takes only constant values)
double val = 5; 
double square_val = square(val); //can be calculated during runtime

//==================================================================================

//mean that this function can be evaluated only during compile time 
consteval double square_eval(double val) {
    return val * val;
}

constexpr double twosquare = square_eval(2); //ok
//double square_val_eval = square_eval(val)


//ok
void ex1() {
    int x = 10;
    const int* p = &x;
    x = 20;
}

//error
void ex2() {
    const int x = 10;
    int* const p = &x;
}


constexpr int twice1(int x) {
    return x * 2;
}

//ok, during runtime
void ex3() {
    int n;
    std::cin >> n;

    int a = twice1(n);
}


consteval int twice2(int x) {
    return x * 2;
}

//invalid, evaluation only during compile time
void ex4() {
    int n;
    std::cin >> n;

    int a = twice2(n);
}

void ex5() {
    int x = 10;
    int* p = &x;

    const int* cp = p; //ok, const protection added

    //===============================================
    int** pp = &p; 
    const int** cpp = pp; //this one is not valid, why? 

    //bad sequence to change const x
    const int immutable = 42;

    int x = 10;
    int* p = &x;
    int** pp = &p;

    // pretend C++ allowed this:
    const int** cpp = pp;

    const int* cp = &immutable;
    *cpp = cp;

    // p is still typed as int*
    *p = 100;   // would modify truly const object 

    //that's why it is invalid 
}

//low level const protection will be losed
void ex6() {
    int x = 10;
    const int* p = &x;

    int* q = p;
}

void ex7() {
    int x = 10;
    int* p = &x;

    int* const q = p;

    *q = 20;
    q = nullptr;
}

void ex8() {
    int x = 10;

    const int* p1 = &x;
    const int* const p2 = p1;

    x = 30;
}

void ex9() {
    int x = 10;

    int* p = &x;
    int** pp = &p;

    int* const* cpp = pp;
}
