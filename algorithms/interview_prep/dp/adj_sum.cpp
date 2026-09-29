#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

//solve(n) = maximum sum using elements from indices 0 to n
//transition =  max((solve(n - 1), solve(n - 2) + adj_n)

// Time:  O(2^n)
// Space: O(n) recursion
int max_adj_sum(const std::vector<int>& nums, int n) {
    if(n < 0) {
        return 0; 
    }

    if(n == 0) {
        return std::max(0, nums[0]);
    }

    return std::max(
        max_adj_sum(nums, n - 2) + nums[n], //take
        max_adj_sum(nums, n - 1)
    ); 
}

// Time:  O(n)
// Space: O(n) memo + O(n) recursion stack
int max_adj_sum_memo(const std::vector<int>& nums, std::vector<int>& memo, int n) {
    if(n < 0) {
        return 0; 
    }

    if(memo[n] != std::numeric_limits<int>::min()) {
        return memo[n];
    }
    int take = max_adj_sum_memo(nums, memo, n - 2) + nums[n];
    int skip = max_adj_sum_memo(nums, memo,  n - 1);

    return memo[n] = std::max(take, skip);
}

int max_adj_sum_dp(const std::vector<int>& nums, int n) {
    if (n == 0) {
        return 0;
    }

    if (n == 1) {
        return std::max(0, nums[0]);
    }

    std::vector<int> dp(n, 0); 
    dp[0] = std::max(0, nums[0]);
    dp[1] = std::max(dp[0], nums[1]);

    for(int i = 0; i < n; i++) {
        dp[i] = std::max(dp[i - 2] + nums[i], dp[i - 1]); 
    }
    return dp[n - 1]; 
}

