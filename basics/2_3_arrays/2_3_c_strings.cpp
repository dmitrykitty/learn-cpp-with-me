#include <iostream>
#include <cstring>
#include <string>

int main() {
    const char* s = "abc";

    std::cout << s << '\n';                 // abc
    std::cout << strlen(s) << '\n';         // 3
    std::cout << sizeof("abc") << '\n';     // 4

    const char* n = "abc\0def";

    std::cout << n << '\n';                 // abc
    std::cout << strlen(n) << '\n';         // 3
    std::cout << sizeof("abc\0def") << '\n'; // 8

    const char* k = "abc";

    std::cout << k << '\n';                 // abc
    std::cout << &k[1] << '\n';             // bc
    std::cout << k[1] << '\n';              // b

    std::cout << static_cast<const void*>(k) << '\n';     // address
    std::cout << static_cast<const void*>(&k[1]) << '\n'; // address of 'b'

    std::string real_string("abc\0def", 7);
    std::cout << real_string.size() << '\n'; // 7
}