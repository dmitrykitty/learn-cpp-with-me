#include <iostream>
#include <algorithm>
#include <vector>

//3.6. Operators Overloading 

class Complex {

    double re = 0.0;
    double im = 0.0;

public:
    Complex(double re): re(re) {}
    Complex(double re, double im): re(re), im(im) {}

    //only for lvalue
    Complex& operator=(const Complex& other) & {
        //....
    }
    //---------------------------------------------------------------
    Complex operator+(const Complex& other) const{
        return Complex(re + other.re, im + other.re);
    }

    Complex& operator+=(const Complex& other){
        this->re += other.re; 
        this->im += other.im; 
        return *this; 
    }

    //---------------------------------------------------------------
    //Three-way comparison (since c++20)
    //automatically will be generated: <, > , <=, >=, ==, != lexigrafically 
    //return type: weak ordering, strong ordering, partial ordering. Tell about it 
    //how to implement own operator <=> by returning less, equal and so on? 
    std::weak_ordering operator<=>(const Complex& o) const = default;
    //if operator spaceship not default -> we should define operator==

    //not needed. usually operator + used += inside
    //friend Complex operator+(const Complex& o1, const Complex& o2); 

    //---------------------------------------------------------------
    //increment
    Complex& operator++() {
        ++re; 
        ++im;
        return *this; 
    }

    Complex operator++(int) {
        Complex copy = *this; 
        ++re; 
        ++im;
        return copy;
    }

    //---------------------------------------------------------------
    double get_re() const { 
        return re; 
    }

    double get_im() const {
        return im;
    }
};

//Return Value Optimization. How it works and why return res+= o2 make one extra copy? 
Complex operator+(const Complex& o1, const Complex& o2) {
    Complex res = o1; 
    res += o2; 
    return res; 
}
//---------------------------------------------------------------
//we only need < to define all other compare operators
//for map and set needed
bool operator<(const Complex& c1, const Complex& c2) {
    return c1.get_re() < c2.get_re() || c1.get_re() == c2.get_re() &&  c1.get_im() < c2.get_im();  
}

bool operator>(const Complex& c1, const Complex& c2) {
    return c2 < c1; 
}

//is it ok? or it's better to define ==? 
bool operator <=(const Complex& c1, const Complex& c2) {
    return c1 < c2 && !(c2 < c1);
}

//---------------------------------------------------------------
// shoul be external function because of it called from ostream 
std::ostream& operator<<(std::ostream& out, const Complex& cmp) {
    return out << cmp.get_re() << ' ' << cmp.get_im();  
}

//---------------------------------------------------------------
//functor
struct Greater {
    bool operator()(int x, int y) const {
        return x > y; 
    }
};

void functor_test() {
    std::vector<int> a = {1, -5, 7, 4}; 
    std::sort(a.begin(), a.end(), Greater());
}

//---------------------------------------------------------------
int main() {

    Complex c1(12.4, 12.3); 
    //By overriding operator + as method we can't sum for example double + complex 
    Complex res = c1 + 3.14; // OK c.operator+(3.14) 
    Complex res2 = 3.14 + c2; //problem, CTE / So code style - binary opewrators shoul be defined out of class 

    // with curretn implementation possible following 
    // c1 + c2 = c3. Why? Bacause by default there is no any block for assigning to rvalue. So we have two solution for this problem: 
    // return const Comples from operator + -> this means we cant change result of the sum. But it's not the best solution 
    // from c++11 we can define for whitch types of value this operator is used. So we need to limit assignment operator only to lvalue by adding & 
}