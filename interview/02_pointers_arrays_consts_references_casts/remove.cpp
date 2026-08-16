#include <iostream>

void update_value(int* first, int* second, int value_to_remove) {
    if(first == second) {
        return;
    }
 
    *first = *second; 
    *second = value_to_remove; 
}

int* remove_value(int* first, int* last, int value) {
    int* second = first;

    while(second < last) {
        update_value(first, second, value);
        if(*first != value) {
            first++;
        }
        second++;
    }
    return first;
}

int main() {
    int arr[]{1, 2, 4};

    int* new_end = remove_value(arr, arr + std::size(arr), 2); 
    for(int* i = arr; i != new_end; i++) {
        std::cout << *i;
    } 
}