#include <cstddef>

void ex1() {
    int a = 10;
    int b = 20;

    int& r = a;
    r = b; //r == a == 20

    b = 30;
}

void ex2() {
    int x = 10;
    const int& r = x;

    x = 20;
}

template<std::size_t N>
void f(int (&arr)[N]) {
    std::cout << N;
}

//reference to array blocl it decay to pointer and save its size
void ex3() {
    int data[17]{};
    f(data);
}