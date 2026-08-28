#include <iostream>

/*
Ambigious call - we are making name lookup and both of the parent has the same name of the function 
*/
namespace EX1 {
    struct A {
        void f(int) {
            std::cout << "A";
        }
    };

    struct B {
        void f(int, int) {
            std::cout << "B";
        }
    };

    struct C : A, B {};

    int ex1() {
        C c;
        c.f(1, 2);
    }
}

/*
Everything ok. Both f functions forms overload set, so for 1- will be called A::f, and for 10, 10 B::f
*/
namespace EX2 {
    struct A {
        void f(int) {
            std::cout << "A";
        }
    };

    struct B {
        void f(int, int) {
            std::cout << "B";
        }
    };

    struct C : A, B {
        using A::f;
        using B::f;
    };

    int ex2() {
        C c;

        c.f(10);
        c.f(10, 20);
    }
}

/*
We cave two A objects
5, 9, false. Object C is shifted, I think aroud 4 bytes
*/
namespace EX3 {
    struct A {
        int x = 1;
    };

    struct B : A {};
    struct C : A {};

    struct D : B, C {};

    int ex3() {
        D d;

        d.B::x = 5;
        d.C::x = 9;

        B* pb = &d;
        C* pc = &d;

        A* ab = pb;
        A* ac = pc;

        std::cout << ab->x << " "
                << ac->x << " "
                << (ab == ac);
    }
}

/*
We can't make direct conversion to not virtual base if we have more than 1 parent because it is ambigious
*/
namespace EX4 {
    struct A {};

    struct B : A {};
    struct C : A {};

    struct D : B, C {};

    int main() {
        D d;

        A* pa = static_cast<A*>(&d);
    }
}

/*
x is single object. So we have only pointer to vtable B-in-A, C-in-A and x field. 
But still pointers for parents are shifted so they are not eq, 
but A is single object and it has definied pointyer, so I guess 42, true
*/
namespace EX5 {
    struct A {
        int x = 1;
    };

    struct B : virtual A {};
    struct C : virtual A {};

    struct D : B, C {};

    int main() {
        D d;

        B* pb = &d;
        C* pc = &d;

        A* ab = pb;
        A* ac = pc;

        ab->x = 42;

        std::cout << ac->x << " "
                << (ab == ac);
    }
}

/*
Most derived class creates base object, so in our case is D. Constructors calls for A from B and C are ignored
*/
namespace EX6 {
    struct A {
        A(int x) {
            std::cout << "A" << x << " ";
        }
    };

    struct B : virtual A {
        B()
            : A(1) {
            std::cout << "B ";
        }
    };

    struct C : virtual A {
        C()
            : A(2) {
            std::cout << "C ";
        }
    };

    struct D : B, C {
        D()
            : A(3),
            B(),
            C() {
            std::cout << "D ";
        }
    };

    int main() {
        D d;
    }
}

/*
We have define at least one custom construcotr in A body, so it means, that no default constructor will be generated. 
So we will get CE
*/
namespace EX7 {
    struct A {
        A(int) {}
    };

    struct B : virtual A {
        B()
            : A(1) {}
    };

    struct C : virtual A {
        C()
            : A(2) {}
    };

    struct D : B, C {
        D()
            : B(),
            C() {}
    };

    int main() {
        D d;
    }
}

/*
I guess it is ambigious because we have A as virtaul base and A as non virtual base. 
*/
namespace EX8 {
    struct A {};

    struct B : virtual A {};
    struct C : A {};

    struct D : B, C {};

    int main() {
        D d;

        A* pa = &d;
    }
}


namespace EX9 {
    struct A {
        static void s() {
            std::cout << "S";
        }

        void f() {
            std::cout << "F";
        }
    };

    struct B : A {};
    struct C : A {};

    struct D : B, C {};

    int main() {
        D d;

        // Analyze these TWO lines independently:
        d.s();
        d.f();
    }
}

/*
static_cast cannot perform a base-to-derived downcast when the base is a virtual base of the target derived class.
Non virtual inheritance has a fixed structural A -> B relationship that static_cast is allowed to invert under its assumptions.
*/
namespace EX10 {
    struct A {};

    struct B : virtual A {};

    int main() {
        B b;
        A* pa = &b;
        B* pb = static_cast<B*>(pa);
        B* pbb = dynamic_cast<B*>(pa);
    }

}

/*
Actually we should do any static cast here, I guess compiler will make implicity cast.  Because order if inheritance is B, A, so object A in C placed after b field. To pass correct implcit this inside class method we need to make shift. So basicly inside aff will be passed this + 4. And this offset is saved inside pointer-to-method thats why its size usually bigger than pointer-to-function
*/
namespace EX11 {
    struct A {
        int a;

        void f() {
            std::cout << "A";
        }
    };

    struct B {
        int b;
    };

    struct C : B, A {};

    int main() {
        void (C::*pf)() =
            static_cast<void (C::*)()>(&A::f);

        C c;

        (c.*pf)();
    }
}

/*
For our case: G2, P, S is printed. Most derived object is responsible for crearting virtual base. 
For Parent p - G1, P because now p is most derived object resposible for creating granny 
*/
namespace EX12 {
    struct Granny {
        Granny(int x) {
            std::cout << "G" << x << " ";
        }
    };

    struct Parent : virtual Granny {
        Parent()
            : Granny(1) {
            std::cout << "P ";
        }
    };

    struct Son : Parent {
        Son()
            : Granny(2),
            Parent() {
            std::cout << "S ";
        }
    };

    int main() {
        Son s;
    }
}

/*
So pb is normally created and a1 too. pc and a2 will also compile. the same for pe and a3. Only last line will not compile because it is ambigious. We have 3 A objects
*/
namespace EX13 {
    struct A {};

    struct B : virtual A {};
    struct C : A {};
    struct E : A {};

    struct D : B, C, E {};

    int main() {
        D d;

        B* pb = &d;
        A* a1 = pb;

        C* pc = &d;
        A* a2 = pc;

        E* pe = &d;
        A* a3 = pe;

        A* direct = &d;
    }
}

/*
A is virtual base so we have single A object. Each change value using B, C or D changes this single A member. 77, 77, true
*/
namespace EX15 {
    struct A {
        int value = 10;
    };

    struct B : virtual A {
        int b = 20;
    };

    struct C : virtual A {
        int c = 30;
    };

    struct D : B, C {
        int d = 40;
    };

    void modify_through_b(B* pb) {
        A* pa = pb;
        pa->value = 77;
    }

    int main() {
        D obj;

        B* pb = &obj;
        C* pc = &obj;

        modify_through_b(pb);

        A* from_b = pb;
        A* from_c = pc;

        std::cout << from_b->value << " "
                << from_c->value << " "
                << (from_b == from_c);
    }
}