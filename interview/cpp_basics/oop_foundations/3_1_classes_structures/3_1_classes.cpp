#include <iostream>

class C; 

struct S; 
//almost the same - in struct everythig public by default 

struct SS {
    int x;
    int y = 1; //default value 
};

//what about size of struct? We can say that size of struct is sum of sizes of its fileds + padding 
struct A {
    int x;
    double y = 1; //default value 
};
// 
//but still why do we have pudding? struct as is putted to the addres divisible by 8 and each dobule the same. 
//so we have following situation
//beggining of te adress divisible by 8 | 4 bytes(int) | padding (to make double address divisible by 8) | 8 bytes(double) |

//inside structure we can define fileds, methods and usings 
struct B {
    int x; 
    #if 0
    using std::cout; //CE - why?
    #endif
    using str = std::string; //OK

    void print_incremented_x(){
        std::cout << x + 1;
    }
};

//==========================================================
//we can define method outside class. also it works for static variables 
struct C {
    std::string name = "name";
    void f();
};

void C::f(){
    std::cout << "Hello " << name;
}


//==========================================================
//this - pointer to object 
struct D {
    std::string name = "name";
    D& f(){
        //this has type D* 
        //so by returning D& i creating new name for *this 
        return *this;
    }
};

//========================================================
//inner struct 
//sizeeof(E) = 16 because no instanses of IE exist
struct E {
    int x; 
    double y; 
    struct IE{
        char c; 
    };
    //I can create instance after its declaration. That you ; reqiared. So it's anonim struct 
    #if 0
    struct {
        char c;
    } c;
    #endif
};


int main(){
    SS s; 
    std::cout << sizeof(SS); //1 if struct is empty 
    std::cout << sizeof(s); //1 if struct is empty 
    //why empty struct or class has 1 byte? To store it inside arrays we need minimal possible step. 

    std::cout << s.x; //UB if not initialized
    std::cout << s.y; //not UB 

    A a{1, 2.5}; //aggregate initialization x = 1, y = 2.5
    A b{1}; //x = 1, y = default value (1)
    //aggregation initialization works when we do not have any constructor and our fileds are private 
    
    //===========================================================
    std::cout << sizeof(E); 
    E::IE ie{'ch'};
    using IE = E::IE; 
    IE ie2; 

    //==========================================================
    //local struct 
    struct S {
        int x = 1; 
        int y = 2; 
    }; 
    S s; // this struct is visible only inside function 
}
