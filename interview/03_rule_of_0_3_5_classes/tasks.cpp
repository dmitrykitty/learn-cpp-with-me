#include <iostream>

/*
A ctor
B ctor
A dtor
*/
namespace EX1 {
    class A {
    public:
        A() {
            std::cout << "A ctor\n";
        }

        ~A() {
            std::cout << "A dtor\n";
        }
    };

    class B {
    public:
        B() {
            std::cout << "B ctor\n";
            throw std::runtime_error("boom");
        }

        ~B() {
            std::cout << "B dtor\n";
        }
    };

    class X {
        A a;
        B b;

    public:
        X() {
            std::cout << "X ctor body\n";
        }

        ~X() {
            std::cout << "X dtor\n";
        }
    };
}

namespace EX2 {
    class C {
    public:
        C() {
            std::cout << "C ctor\n";
        }

        ~C() {
            std::cout << "C dtor\n";
        }
    };

    class B {
        C c;

    public:
        B() {
            std::cout << "B ctor body\n";
            throw std::runtime_error("boom");
        }

        ~B() {
            std::cout << "B dtor\n";
        }
    };
}

/*
A(1) ctor
A(2) ctor
X ctor body
X dtor body
A(2) dtor
A(1) dtor
*/
namespace EX3 {

class A {
    int id_;

public:
    A(int id) : id_(id) {
        std::cout << "A(" << id_ << ") ctor\n";
    }

    ~A() {
        std::cout << "A(" << id_ << ") dtor\n";
    }
};

class X {
    A first_;
    A second_;

public:
    X()
        : second_(2),
          first_(1) {
        std::cout << "X ctor body\n";
    }

    ~X() {
        std::cout << "X dtor body\n";
    }
};
}
