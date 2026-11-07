#include <bits/stdc++.h>

using namespace std; 

void rebalanse(multiset<int, greater<>>& left, long long& lsum, multiset<int>& right, long long& rsum) {
        if(left.size() > right.size()) {
            int largest_left = *left.begin(); 
            lsum -= largest_left; 

            left.erase(left.begin());
            right.insert(largest_left);
            rsum += largest_left; 
            return; 
        }

        if(right.size() - left.size() >= 2) {
            int smallest_right = *right.begin(); 
            rsum -= smallest_right; 

            right.erase(right.begin());
            left.insert(smallest_right);
            lsum += smallest_right; 
        }
    }

long long minOperations(vector<int>& nums, int x, int k) {
        long long lsum = 0, rsum = 0; 
        int start = 0;
        vector<long long> costs;
        costs.reserve(nums.size() - x + 1);  

        multiset<int, greater<>> left; 
        multiset<int> right; 

        for(int end = 0; end < nums.size(); ++end) {

            //update window 
            if(!left.empty() && *left.begin() > nums[end]) {
                left.insert(nums[end]); 
                lsum += nums[end]; 
            } else {
                right.insert(nums[end]);
                rsum += nums[end];
            }

            //shrink if needed
            if(end - start + 1 > x) {
                if(*right.begin() > nums[start]) {
                    left.erase(left.find(nums[start]));
                    lsum -= nums[start]; 
                } else {
                    right.erase(right.find(nums[start]));
                    rsum -= nums[start]; 
                }
                start++; 
            }

            rebalanse(left, lsum, right, rsum);

            //add new value 
            if(end + 1 >= x) {
                long long med = *right.begin(); 
                costs.push_back(med * left.size() - lsum + rsum - med * right.size()); 
            }
        }

        vector<vector<long long>> dp(costs.size()+x, vector<long long>(k+1, 0));

        for(int i = 0; i < x; ++i) {
            for(int j = 0; j < k+1; ++j) {
                dp[i][j] = LLONG_MAX / 2; 
            }
        }

        for(int i = 0; i < dp.size(); ++i) {
            dp[i][0] = 0; 
        }

        for(int i = x; i < dp.size(); ++i) {
            for(int j = 1; j < k+1; ++j) {
                long long take = costs[i-x] + dp[i-x][j-1];
                long long not_take = 0 + dp[i-1][j]; 

                dp[i][j] = min(take, not_take); 
            }
        }
        return dp[costs.size()-1+x][k]; 
    }