#include <bits/stdc++.h>

using namespace std; 

class Solution {
public:
    int splitArray(vector<int>& nums, int k) {
        int left = 0;
        int right = 0; 

        for(int n: nums) {
            left = max(left, n);
            right += n;
        }

        while(left < right) {
            int mid = left + (right - left) / 2; 

            if(is_suitable_sum(nums, mid, k)) {
                right = mid; 
            } else {
                left = mid + 1; 
            }
        }

        return right; 
    }

private: 
    bool is_suitable_sum(const vector<int>& nums, int sum, int limit) {
        int needed = 1;
        int cur_sum = 0; 
        for(int num: nums) {
            if(cur_sum + num > sum) {
                needed++;
                if(needed > limit) {
                    return false; 
                } 
                cur_sum = 0;
            }
            cur_sum += num; 
        }
        return true; 
    }
};