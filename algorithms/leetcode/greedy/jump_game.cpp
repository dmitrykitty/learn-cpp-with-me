#include <bits/stdc++.h>

using namespace std; 

bool canJump(vector<int>& nums) {
        int max_available_idx = 0; 

        for(int i = 0; i < nums.size(); ++i) {
            if(max_available_idx >= nums.size() - 1) {
                return true; 
            }

            if(max_available_idx < i) {
                return false;
            }

            max_available_idx = max(max_available_idx, i + nums[i]); 
        }
        return true;
    }