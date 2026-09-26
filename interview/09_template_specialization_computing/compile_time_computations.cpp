#include <iostream>

namespace FIB {
//takes O(N) instead of O(2^N) because class<N> generated only once 
template <int N>
struct Fibbonaci {
    static constexpr int value = Fibbonaci<N - 1>::value + Fibbonaci<N - 2>::value; 
};

template <>
struct Fibbonaci<1> {
    static constexpr int value = 1; 
};

template <>
struct Fibbonaci<0> {
    static constexpr int value = 0; 
};
} //FIB

namespace NUMBERS {

template <int N, int D>
struct IsPrimerHelper {
    static constexpr bool value = N % D == 0 ? false : IsPrimerHelper<N, D - 1>::value; 
};

template <int N>
struct IsPrimerHelper<N, 1> {
    static constexpr bool value = true; 
};



template <int N>
struct IsPrime {
    static constexpr bool value = IsPrimerHelper<N, N / 2 + 1>::value; 
};

template <>
struct IsPrime<2> {
    static constexpr bool value = true; 
};

template <>
struct IsPrime<1> {
    static constexpr bool value = false; 
};

template<>
struct IsPrime<0> {
    static constexpr bool value = false;
};

template <int N>
constexpr bool is_prime_v = NUMBERS::IsPrime<N>::value; 


} //NUMBERS

namespace FAC {

template <int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value; 
};

template<>
struct Factorial<0> {
    static constexpr int value = 1; 
};
} //FAC




int main() {
    std::cout << FIB::Fibbonaci<20>::value << '\n'; 
    std::cout << NUMBERS::IsPrime<257>::value << '\n'; 
    std::cout << NUMBERS::is_prime_v<257> << '\n';
    std::cout << FAC::Factorial<5>::value << '\n'; 
}