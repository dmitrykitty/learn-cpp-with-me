#include <iostream>

int main(){

    if(/* bool-expression or declaration since C++ 17 */true){

    }else if (/* bool-expression */true){

    }else {

    }

    //===========================================
    int x = 5; 
    //better optimalization by compilers so preffered if we have a lot statements
    switch(/*expression integral type*/x){
    case 1:
        std::cout << 1; 
        break; 
    case 2:
        std::cout << 2; 
        break; 
    default:
        std::cout << x; 
    }

    //===========================================
    while(true){}
    
    do{

    }while(true);
    
    for(/*definition | expression*/; /*bool_expression*/; /*expression*/){
        continue;
        break;
    }

    //===========================================

label:
    x++; 
    std::cout << x; 
    goto label; 
}