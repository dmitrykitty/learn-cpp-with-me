#include <bits/stdc++.h>

using namespace std; 

int removeElement(vector<int>& nums, int val) {
    int target_idx = 0;
    for(int i = 0; i < nums.size(); i++){
        int temp = nums[i]; 
        nums[i] = nums[target_idx];
        nums[target_idx] = temp;

        if(nums[target_idx] != val){
            target_idx++; 
        }
    }
    return target_idx;
}