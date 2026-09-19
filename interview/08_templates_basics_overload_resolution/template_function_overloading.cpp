#include <iostream>
#include <limits>

//Main rule - more specific function is more preferable 
//instantation - generatig specialization by compiler from template and template's arguments
//implicit - compiler decide what specialization be generated
//explicit - you can ask to generate this specialization 
//instantiation is lazy - so until you use bad example - no CE occured

template <typename T>
void f(T x) {
    std::cout << 1;
}

template 
void f<int>(int x); //explicit instantiation

template <>
void f<int>(int x) {} //explicit specialization



void f(int x) {
    std::cout << 2;
}

//redefinition
int k() {
    return 1;
}
//double k(){}

//For function templates, the return type participates in determining the template's signature/equivalence.
//so ok - but after calling -> ambigious call 
template<typename K>
int r(K k){
    return k;
}

template<typename K>
double r(K k){
    return k;
}

//-----------------------------------------------------
//default arguments 
template <typename T = int> 
T return_max() {
    return std::numeric_limits<T>::max();
}

//-----------------------------------------------------
//return type could be deduced, so only param type concidered
//even if it obvious 
template<typename T, typename U>
U ff(T x) {
    return 0; 
}

template<typename U, typename T>
U fff(T x) {
    return 0; 
}


//overloading with references
//ambigious call 
//for references we have different rules: 
//  reference is just alias for normal variable 
//  so in templates template from T& and T is the same 
//  and call of this functions is ambigious 

//between T and T& nothing is better -> ambigious
//betwqeen T and const T nothing is better -> ambigious 
template <typename T>
void rr(T& x) {
    std::cout << 1; 
}

template <typename T>
void rr(T x) {
    std::cout << 2; 
}



int main() {
    int x = 1; 
    f(x); //2
    f(2L); //1
    f<int>(2L); //implicit cast long -> int 1 
    //---------------------------------------------------------------------
    std::cout << '\n';
    std::cout << return_max() << '\n'; // by default int max - 2147483647
    std::cout << return_max<long>(); //long max 9223372036854775807

    //--------------------------------------------------------
    //two types required
    ff<int, double>(0);
    //only one type needed fo return value 
    fff<int>(0); 

    //-------------------------------------------------------------------

    int y = 0; 
    //rr(y); //ambigious call 
    rr(1); //it's ok, because 1 is rvalue and we cant have T& to rvalue 1
    //but for const T& it is ambigious again 
}