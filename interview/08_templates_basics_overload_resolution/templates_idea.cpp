#include <iostream> 
#include <map>

//template function 
template<typename T>
T sum(T a, T b) {
    return a + b; 
}

//template class 
template<typename T>
class Vector {
    T* data; 
    size_t sz; 
    size_t cap; 
};

//template using (if we have vecry long name)
template<typename T>
using mymap = std::map<T, T, std::greater<T>>;

template<typename T>
constexpr T num = 12 + 8; 


template<typename T, typename U>
void f(T t, U u) {
    std::cout << "1";
}

template<>
void f<int>(int a, int b) {
    std::cout << "3";
}

template<typename T>
void f(T t, T tt) {
    std::cout << "2";
}




int main() {
    int a; 
    long b; 
    //sum(a, b); //CE because we need to decide wfunction with which type should be generated
    sum<long>(a, b); //ok 

    int c; 
    f(a, c); 

    auto k = num<double> * 5; 
    std::cout << typeid(k).name(); //d - double 

    //vector<int> and vector<double> - two different generated classes, so we can assign one to other
}