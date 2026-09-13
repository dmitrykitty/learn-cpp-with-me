#include <vector>
#include <iostream>

void print_vector(std::vector<int>& v, int start, int end, std::string_view prefix) {
    std::cout << prefix << ": ";
    for(int i = start; i < end; i++) {
        std::cout << v[i] << " "; 
    }
    std::cout << '\n';
}

void merge(std::vector<int>& nums, int lbegin, int middle, int rend) {
    int n = rend - lbegin;
    std::vector<int> tmp(n); 

    int i = 0, j = lbegin, k = middle; 
    while(j < middle && k < rend) {
        if(nums[j] < nums[k]) {
            tmp[i] = nums[j];
            j++;
        } else {
            tmp[i] = nums[k];
            k++;
        }
        i++;
    }

    while(j < middle) {
        tmp[i] = nums[j];
        i++;
        j++;
    }

    while(k < rend) {
        tmp[i] = nums[k];
        i++;
        k++; 
    }

    for(int i = 0; i < n; i++) {
        nums[lbegin + i] = tmp[i];
    }
}

//[start, end)
//so end == size of vector
void merge_sort(std::vector<int>& nums, int start, int end) {
    if(end - start <= 1) {
        return;
    }

    int middle = start + (end - start) / 2; 
    print_vector(nums, start, middle, "left");
    merge_sort(nums, start, middle);
    print_vector(nums, start, middle, "sorted left");
    print_vector(nums, middle, end, "right");
    merge_sort(nums, middle, end);
    print_vector(nums, middle, end, "sorted right");
    merge(nums, start, middle, end);
}

int main() {
    std::vector<int> v = {5, 1, 3, 2, 8, 4, 12, 6}; 
    merge_sort(v, 0, v.size()); 

    print_vector(v, 0, v.size(), "result");

}