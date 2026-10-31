#include <bits/stdc++.h>

using namespace std; 


bool canPartition(vector<int>& nums) {
        int sum = 0; 
        for(int num : nums) {
            sum += num; 
        }

        if(sum % 2 == 1) {
            return false; 
        }
        int target = sum / 2;
        vector<bool> dp(target + 1, false); 

        for(int t = 0; t <= target; ++t){
            dp[t] = (nums[0] == t);
        }

        for(int idx = 1; idx < nums.size(); ++idx) {
            for(int t = target; t >= 0; --t) {
                if(nums[idx] == t) {
                    dp[t] = true; 
                    continue;
                }

                bool not_take = dp[t]; //stil value frpm prev iteration
                bool take = false; 
                if(nums[idx] <= t) {
                    take = dp[t - nums[idx]]; 
                }
                dp[t] = take || not_take; 
            }
        }
        return dp[target];
    }