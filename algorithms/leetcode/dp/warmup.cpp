#include <bits/stdc++.h>
#include <climits>


int f(const std::vector<int>& gift, std::vector<int>& memo, int step) {
    if(step < 0) {
        return INT_MAX; 
    }

    if(step == 0) {
        return gift[0]; 
    }

    if(memo[step] != -1) {
        return memo[step];
    }

    return memo[step] = gift[step] + std::min(f(gift, memo, step - 1), f(gift, memo, step - 3)); 
}

int f_dp(const std::vector<int>& gift) {
    int n = gift.size(); 
    if(n == 0) {
        return 0;
    }
    
    std::vector<int> dp(n); 
    dp[0] = gift[0]; 
    for(int i = 1; i < n; ++i) {
        int big_jump = i - 3 < 0 ? INT_MAX : dp[i - 3]; 
        dp[i] = gift[i] + std::min(dp[i - 1], big_jump);
    }
    return dp[n - 1]; 
}