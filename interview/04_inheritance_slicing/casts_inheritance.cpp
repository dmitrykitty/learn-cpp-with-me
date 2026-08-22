#include <iostream>

struct Base {
    int x = 1;
};

struct Derived: Base {
    int y = 2;
};

void fref(const Base& b) {
    std::cout << "Base&" << b.x;
}

void fcopy(Base b) {
    std::cout << "Base" << b.x;
}

void fpointer(Base* b) {
    std::cout << "Base*" << b->x;
}

int main() {
    const Derived cd; 
    fref(cd);

    //Slicing - creating Base from Derived just cutting out Derived part 
    Derived d; 
    fcopy(d);

    Derived dp; 
    fpointer(&dp);
}