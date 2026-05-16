#include <iostream>
#include <vector>
// 1.6. Compile-time errors, runtime errors, UB

/*
3 types of compile-time errors:
    -lexical
    sonmething whjat parser didt'n understand for example \\3;
    -sintax
    for example 5 + and here nothing, so secong expression is missed ofr binary operator +
    -semantic
    for example "abc" + 5 or ++x++

runtime errors:
    - segmentation fault - access to memory whitch is unvailable for as
    - floating point
    - aborted
*/
int main(){
    //==================RUNTIME EXCEPTIONS===================
    // SEGMENTATION FAULT
    std::vector<int> v(10);
    v[500] = 1; // why here is no seg fault? as I now it connected with how OOS is work witgh memory, but explain it better
    // v[50'000] = 1; //Segmentation fault (core dumped)

    // FLOATING POINT
    int y = 1;
    std::cout << y / 0; // Floating point exception (core dumped) tell more about prcessor error and how it works. Why 5.0 / 0 is inf?

    // Aborted (core dumped)
    v.at(10);

    v[-1] = 1000;
    // what does kept if vector in -1 element? why v[-1] gives dobule free or corruption Aborted?

    //====================UB======================
    int x;
    std::cout << x; // UB

    x++ + ++x; // UB (order is undefined)

    //what is core dumb? some flag for compilator? how to use it? 

    //the as-if rule and how does it work 
}