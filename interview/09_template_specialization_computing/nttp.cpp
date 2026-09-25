#include <iostream>

//Non type template parameters
//arr<int, 100> and arr<int, 99> it is two different classes 
template <typename T, size_t N >
class array {
    T arr[N];
};

template <size_t M, size_t N, typename Field = double>
class Matrix {};

//template using, so we can call SquareMatrix<int, 5> -> Matrix<int, 5, 5> 
template <size_t N, typename Field = double>
using SquareMatrix = Matrix<N, N, Field>; 

template <size_t M, size_t K, size_t N, typename Field = double>
Matrix<M, K, Field> mul(
    const Matrix<M, N, Field>& A, 
    const Matrix<N, K, Field>& B
) {/*...*/}

int main() {
    array<int, 100> arr; 

    Matrix<4, 5> m1; 
    SquareMatrix<5> m2; 
    Matrix<4, 8> m3;

    mul(m1, m2); //ok
    mul(m1, m3); //CE

    //Important note - template param should beknown in compile time
    //so
    size_t x = 5;
    Matrix<x,x> m4; //CE - expression must have a constant value

    const size_t y = 5; 
    Matrix<y, y> m5; //OK

    size_t k;
    std::cin >> k;
    const size_t constK = k; 
    Matrix<constK, constK> matrix6; //CE because value of constK wasn't known during CT

    //so if we want to prevent usage of values that are runtime
    size_t p;
    std::cin >> p;
    constexp size_t constKE = p;  //CE already here (constexpr from C++11)


}

// Template template parameters
//For example adaptros over containers (stack, deque) takes second param - container
//but maybe they take type, not template because of allocator...?
template<
    typename T,
    template<typename, typename> class Container = std::vector
>
class stack {
    Container<T, std::allocator<T>> cont;
};


namespace EX1 {
template<std::size_t N>
struct Buffer {};

constexpr std::size_t getFive() {
    return 5;
}

std::size_t runtimeFive() {
    return 5;
}

int main() {
    const std::size_t a = 5;
    const std::size_t b = getFive();
    const std::size_t c = runtimeFive();

    constexpr std::size_t d = getFive();

    Buffer<a> x1; //cont a = 5
    Buffer<b> x2; //const b = constexpr function call
    Buffer<c> x3; //CE
    Buffer<d> x4; //ok, constexpr c = constexpr function call 
}
} //EX1

namespace EX2 {
#include <type_traits>

template<auto N>
struct X {};

int main() {
    X<5> a; //int
    X<5u> b; //uint
    X<5L> c; //long
    X<'\5'> d; //char
    //even if all this types are integral -> they are different types 
    static_assert(std::is_same_v<decltype(a), decltype(b)>);
    static_assert(std::is_same_v<decltype(a), decltype(c)>);
    static_assert(std::is_same_v<decltype(a), decltype(d)>);
}
} //EX2

namespace EX3 {
template<
    std::size_t R,
    std::size_t C,
    typename T = double
>
struct Matrix {};

template<
    std::size_t M,
    std::size_t N,
    std::size_t K,
    typename T
>
Matrix<M, K, T>
mul(
    const Matrix<M, N, T>& lhs,
    const Matrix<N, K, T>& rhs
);

int main() {
    Matrix<3, 4, double> a;
    Matrix<4, 7, double> b;
    Matrix<5, 7, double> c;
    Matrix<4, 7, float>  d;

    auto x1 = mul(a, b); //ok
    auto x2 = mul(a, c); //fail 4 != 5
    auto x3 = mul(a, d); //fail double != float
}
} //EX3

namespace EX4 {
template<unsigned N>
struct A {};

template<int N>
struct B {};

int main() {
    A<5> a1; //ok
    A<-1> a2; //-1 is not unsigned: requires converting -1 to unsigned, but this is a narrowing conversion in this context

    B<5u> b1; //ok
    B<4'000'000'000u> b2; //to big for int, 2'147'...'...
}
}

/*
template<int N>   -> identity of the integer value
template<int* P>  -> identity of the pointer value/object address
template<int& R>  -> identity of the referenced object
*/
namespace EX5 {
template<int* P>
struct Ptr {};

int x = 42;
int y = 42;

template<int& R>
struct Ref {};

int main() {
    Ptr<&x> a;
    Ptr<&y> b;
    //we have different integral value, so it is like for arrays array<5> != array<6>
    static_assert(std::is_same_v<decltype(a), decltype(b)>);

    Ref<x> r1;
    Ref<y> r2;
    static_assert(std::is_same_v<decltype(r1), decltype(r2)>); //I think it should compile because of equal value of x and y
}
} //EX5

namespace EX6 {
template<const int* P>
struct Holder {};

int global = 10;

int main() {
    int local = 10;
    static int staticLocal = 10;

    Holder<&global> a; //static storage duration
    Holder<&local> b; //automatic storage duration
    Holder<&staticLocal> c;//static storage duration
}
} //EX6

//Conceptually, NTTPs need stable compile-time identity because:
namespace EX7 {
template<const char* Name>
struct Tag {};

constexpr char foo[] = "foo";

int main() {
    Tag<foo> x;
    Tag<"foo"> y; //fails
}
} //EX7


namespace EX8 {

/*
structural class (from c++20)
it is a literal class,
its relevant members are public,
they aren't mutable,
and their types are themselves valid structural types.
*/
struct Shape {
    std::size_t rows;
    std::size_t cols;

    constexpr bool operator==(const Shape&) const = default;
};

template<Shape S>
struct Matrix {
    static constexpr std::size_t rows = S.rows;
    static constexpr std::size_t cols = S.cols;
};

int main() {
    Matrix<Shape{2, 3}> a;
    Matrix<Shape{2, 3}> b;
    Matrix<Shape{3, 2}> c;

    static_assert(std::is_same_v<decltype(a), decltype(b)>); // yes
    static_assert(std::is_same_v<decltype(a), decltype(c)>); // no
}
} //EX8

// :( .....
namespace EX9 {
template<std::size_t N>
struct Vec {};

template<std::size_t N>
void f(Vec<N>) {}

template<std::size_t N>
void g(Vec<N + 1>) {}

int main() {
    Vec<5> v;

    f(v);
    //the compiler does not algebraically solve template argument expressions.
    g(v); //fails
    //but
    g<4>(v); //ok
}
} //EX9

namespace EX10 {
template<std::size_t N>
struct Vec {};

template<std::size_t N>
//here N+1 can be deduced because N already known from Vec<N>
void h(Vec<N>, Vec<N + 1>) {}

int main() {
    Vec<4> a; // class Vec4 generated
    Vec<5> b; //class Vec5 generated
    Vec<6> c; // class Vec6 generated


    h(a, b); //ok
    h(b, c); //ok

    //Vec<4>, Vec<5> expected
    h(a, c); //no function for Vec<N>, Vec<N + 2> provided
}
} //EX10

