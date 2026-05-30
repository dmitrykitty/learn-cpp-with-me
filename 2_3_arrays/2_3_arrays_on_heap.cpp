#include <iostream>

//array in dynamic memory 
int main(){
    int* a = new int[12]; //random values
    int* b = new int[5]{}; //0 0 0 0 0 

    //delete a - UB. before a keept some metadata of how much memory should be deleted, so calling delete a we are ignoring this data 
    delete[] a; 
    delete[] b;
}