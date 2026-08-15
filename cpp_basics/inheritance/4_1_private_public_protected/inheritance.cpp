#include <iostream>

// 4.1. public, private, protected inheritance 

namespace EX1 {
    struct Base {
    private: 
        int y = 5; //available for all other members of this class, friends
    protected:
        int x = 6; //available for all other members of this class, friends and INSIDE derived classes
    public: 
        void f() {
            std::cout << y; 
        }
    };

    struct Derived: Base {
        int y;
        void g() {
            std::cout << x; // prints x from Base, it's like my own field 
        } 
    };
}
//===============================================

namespace EX2 {
    struct Base {
        int x = 6;
        void f() {
            std::cout << x; 
        }
    };

    //public: everyones knows that Derived is child of Base
    //private: only me and my friends knows that Derived is child of Base
    struct Derived: private Base {
        int y;
        void g() {
            std::cout << x;
        } 
    };

    void private_inheritance_test() {
        //x from Base is public but inheritance is private, so inside this function we don't now thar Derived is child of Base
        Base b; 
        std::cout << b.x << std::endl; //ok, because x is public 

        Derived d; 
        // std::cout << d.x <<std::endl; //CTE - x is invisible for this function 
        /*
        we can think about inherit mod and access mod like about two doors: 
            - firstly we should see that class is derivable 
            - secondly base class should have available fields 
        */
        
    }

    //=======================================

    struct Granny {
    private:
        int p_x;
    protected:
        int prot_x; 
    public:
        int x = 1;
        void f(){}
    };

    struct Mom: protected Granny {
    private:
        int p_y = 4;
    public: 
        int y = 2; 
        void g(){}

        //so it allows to see all private members of mom from main
        //because of public inheritance now we can see all private fileds of mom from son 
        friend void protected_inheritance_test();
    };

    struct Son: Mom {
        int z = 3;
        void h(){
            std::cout << z; //ok
            std::cout << y; //ok 
            std::cout << x; //ok
        }

        void g_h(Granny& granny) {
            std::cout << granny.prot_x; //Not ok, wee have acces only to this protected fileds, but here is granny protected field 
            std::cout << granny.x; 
        }
    }; 

    void protected_inheritance_test() {
        Son s;
        std::cout << s.z; //ok
        std::cout << s.y; //ok
        // std::cout << s.x; //not ok, only inside Son we can see that Mom is derivied from Granny
        std::cout << s.p_y; //because of friendship <3

        Mom m; 
        //std::cout << m.x; //CE, because of protected 
        
    }
    


}



int main() {
    std::cout << sizeof(EX1::Base) << " " << sizeof(EX1::Derived) << std::endl; 
    EX1::Derived b;
    b.f();  
    //b.x; CE - not available from main

    /*
    Visible and available it is not the same. Firstly compiler choose function/variable from scopes, for example x even if it private 
    Just later he checks is it AVAILABLE and can be used 
    */
   //============================================
}