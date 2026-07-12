#include <iostream>
#include "String.h"

int main() {
    String s = "deddabcd"; 
    String substr = "dd"; 

    std::cout << s.find(substr) << '\n'; 
    std::cout << "substr\n";

    String a = "DimaLoxaaaaa"; 
    std::cout << a.substr(4, 6) << '\n'; 
    std::cout << a.substr(4, 2);
}