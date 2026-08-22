#include <iostream>

namespace EX1 {
struct Base {
protected:
    int x = 10;

public:
    Base() {
        std::cout << "Base()\n";
    }

    Base(const Base& other)
        : x(other.x) {
        std::cout << "Base copy\n";
    }

    void f(int) {
        std::cout << "Base::f(int)\n";
    }

    void f(int, int) {
        std::cout << "Base::f(int,int)\n";
    }
};

struct Derived : private Base {
    int x = 20;
    int y = 30;

    using Base::f;

    Derived() {
        std::cout << "Derived()\n";
    }

    void f(double) {
        std::cout << "Derived::f(double)\n";
    }

    void test(Base& b) {
        // LINE A
        // std::cout << b.x << '\n';

        // LINE B
        std::cout << Base::x << '\n';
    }
};

void consume(Base b) {
    std::cout << "consume\n";
}

int ex1() {
    Derived d;

    d.f(1);
    d.f(1, 2);
    d.f(1.5);

    // LINE C
    // Base* p = &d;

    // LINE D
    // consume(d);

    /*
    Exact constructor output for Derived d. Base, Derived
    Which overload each of the three d.f(...) calls selects, and why.
    Whether line A compiles.
    Whether line B compiles.
    Whether line C compiles.
    Whether line D compiles.
    For every compile error, explain which mechanism causes it: lookup, overload resolution, access control, or something else.
    */
}
}

namespace EX2 {
struct Message {
    int timestamp;

    void print(int level) {
        std::cout << "Message\n";
    }
};

struct OrderMessage : Message {
    int order_id;
    double price;

    void print() {
        std::cout << "Order " << order_id << '\n';
    }
};

void log_message(Message msg) {
    msg.print(1);
}

void process(OrderMessage& msg) {
    msg.print();
}   

void process(const Message& msg){}

}