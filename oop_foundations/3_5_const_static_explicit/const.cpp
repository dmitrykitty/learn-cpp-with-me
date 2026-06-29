#include <iostream>

//CONST AND STATIC 
struct S {
    void f() {
        std::cout << "Hi";
    }
};

struct S2 {
    //what going on with attributes for const object 
    //for example for char* arr; only pointer will be const, not each char -> char* const arr; 
    char arr[10];

    S2() = default; 
    
    //what about key word mutable 
    mutable int p = 0; 

    //but for 
    void f() const {
        std::cout << "Hi";
    }

    //why not simple char 
    //we can change selected char by another variable by we still should see this these changes by our reference 
    const char& operator[](size_t index) const {
        return arr[index];
    }
    char& operator[](size_t index) {
        return arr[index];
    }

    //as we remember we ccan't change fields in const methods because of const this 
    //but it doesnt work for references 

    int x = 0; 
    int& xr = x; 

    void f2(int y) const { 
        x++; // CTE 
        xr++; // OK. Why doest it work? Is it ub? 
    }
};

struct StaticExample{
    //const not reqired
    // where Pi is saved? in bss or data 
    static double PI; 
    inline static double pi = 5; 
    //why only const if initialized 
    const static double PI_INIT = 3.1415926535;
    //what the diff between 3 versions  


    static void increment(int& x) {
        ++x; 
    }

};
double StaticExample::PI = 3.1415;



void tests(){
    const S s;
    s.f(); //CTE cont required for method f() inside S  

    const S2 s2; 
    s2.f(); //OK 

    //so we can't call any non cont methods using cont instance of the object 
    //by default each method gets this and by adding conts this will be const this
    //so without this im method we are trying to non const this by making const cast  
}

void test_static() {
    int x = 1; 
    StaticExample::increment(x);
    std::cout << x; 
    std::cout << StaticExample::PI; 
}


int main(){

}