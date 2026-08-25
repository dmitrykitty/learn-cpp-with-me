#include <iostream>

struct Granny {
    int g;

    explicit Granny(int value)
        : g(value) {
        std::cout << "Granny(" << value << ")\n";
    }
};

struct Parent1 : virtual Granny {
    int p1;

    explicit Parent1(int value)
        : Granny(100 + value),
          p1(value) {
        std::cout << "Parent1, g = " << g << '\n';
    }
};

struct Parent2 : virtual Granny {
    int p2;

    explicit Parent2(int value)
        : Granny(200 + value),
          p2(value) {
        std::cout << "Parent2, g = " << g << '\n';
    }
};

struct Son : Parent1, Parent2 {
    int s;

    Son()
        : Granny(999),
          Parent1(1),
          Parent2(2),
          s(3) {
        std::cout << "Son, g = " << g << '\n';
    }
};

int main() {
    Son s; 
    std::cout << sizeof(s); 

    Parent1 p1(1);
    Granny g(1);

    std::cout << sizeof(p1) << sizeof(g);
}