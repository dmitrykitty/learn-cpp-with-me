#include <iostream>
#include <vector>

//Metafunctions are based on structsd and specializations
/*
Usefull functions: 
    -is_array
    -rank - dimension of array 
    -decay - remove all modifiers (references, const, volatile,...)
    -conditional
*/
template<typename T, typename U>
struct is_same {
    static constexpr bool value = false; 
};

template<typename T>
struct is_same<T, T> {
    static constexpr bool value = true; 
};

template<typename T, typename U>
constexpr bool is_same_v = is_same<T, U>::value; 

template<typename T, typename U>
void f(T x, const U y) {
    // some statements
    if(typeid(T) == typeid(y)) {
        //this option is not very good - we don't want to add runtime check overhead when we can do it in compile time 
    }

    if constexpr(is_same<T, U>::value) {
        x = y; //without const expr this will not compile. After adding constexpt if computed in compile time as well.
        //So if condition is false -> it will not compile -> no error
        std::cout << "equal\n";
    } else {
        std::cout << "not equal\n"; 
    }
    // some statements

    if constexpr(is_same_v<T, U>) {
        //some statements
    }

    //some statements
}

//---------------------------------------------------
template<typename T>
struct remove_reference {
    using type = T; 
};

template<typename T>
struct remove_reference<T&> {
    using type = T; 
};

template<typename T>
using remove_reference_t = typename remove_reference<T>::type; 

template<typename T>
void ff(T t) {
    typename remove_reference<T>::type tt = t; 
    remove_reference_t<T> ttt = t; 
    std::cout << typeid(ttt).name() << "\n";
}
//---------------------------------------------------

template<typename T>
struct remove_const {
    using type = T; 
};

template<typename T>
struct remove_const<const T> {
    using type = T;
};

template<typename T>
using remove_const_t = typename remove_const<T>::type; 

//---------------------------------------------------

template<bool B, typename T, typename F>
struct conditional {
    using type = F;
};

template<typename T, typename F>
struct conditional<true, T, F> {
    using type = T; 
};

template<bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type; 


int main() {
    std::cout << is_same<std::vector<int>, std::vector<double>>::value; 
    f(5, 5);
    ff(5); 
}