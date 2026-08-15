#include <iostream>

// 1.3. Declarations, definitions, scopes and usingnames

int a = 5; //by default it's 0 

//declarations 
void f(int x);
void fun();
void ffun();

class C; 

struct S; 

union U; 

namespace N {
    int a;
    int b; 
}

namespace M {
    int c; 
    int d; 
}


/*
3 ways of using: 
    - using name_of_variable
    - using namespace name_of_namespace
    - using vi = std::vector<int> as alias (more readable then typedef: typedef vector<int> vi)

after C++ 17 we can use multiple using separated by comma: 
    using std::cout, std::cin

from C++17 we can create multiple namespace namespace N::NN::NNN{ }
*/


int main() {
    N::b = 6; 

    //to get access to the a withount naming namespace each time
    using N::a; 
    a = 10; 
    std::cout << a << std::endl;
    
    //or can be used in global scope for global visibility 
    //using namespace std is not reccomended in global scope because of name conflict 
    //(for example distance function)
    using namespace M; 
    c = 12; 
    d = 13; 

    std::cout << c << " " << d << std::endl; 

    fun(); //2 0
    ffun();
}

/*
x = 0 - global scope 
x = 1 - function scope 
x = 2 - loop scope 

rule: variable taken from most local name

to get x from global scope if more local x cover it - ::x
*/
int x = 0; 
void fun(){
    int x = 1; 

    for(int i = 0; i < 1; ++i){
        int x = 2; 
        std::cout << x << ' ' << ::x << std::endl; 
    
    }
}

namespace D {
    int r = 2; 
}
namespace E {
    int g = 1; 
}
int g = 0; 

using namespace D; 
using namespace E; 

void ffun(){
    std::cout << "r from namespace D(2): gotten by ::r " << ::r << std::endl; 
    std::cout << "g from global scope (0), has higher priority then g from name space " <<  ::g << std::endl; 
}

namespace NN1 {
    int num = 1;
}
namespace NN2 {
    int num = 2; 
}
void ref_is_ambigious_error(){
    using namespace NN1; 
    using namespace NN2; 
    //error - reference to num is ambigious - impossible to recognize which one to use 
    //but if isntead of one namespace we will use directly varfiable name: using NN2::num - everything ok 
    std::cout << num; 
}

//ODR - One Definition Rule (each entity should be defined only once) checked in the time of linking 
//(what is difference of definition, declaration, inicialization)
////every definition is a declaration 
//but class can be defined multiple times in whole program if defined in exactly same way but only one time in one translation unit(??? explain it)

//if we want declare variable, but do not define it - key word extern 


//propomotion vs convertion (expanding type is more preferable then conversion)
int f1(double x){
    return x + 1; 
}

int f1(int x){
    return x + 2; 
}

int f2(float x){
    return x + 1; 
}

int f2(int x){
    return x + 2; 
}

int function_manager(){
    std::cout << f1(0.0); // perfect matching with double 1
    std::cout << f1(0.0f); //promotion to double 1 

    std::cout << f2(0.0); //compilation error - two options with int and float the same bad  
    //(more than one instance of overloaded function "f2" matches the argument list)
}












