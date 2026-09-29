#include <algorithm>
#include <vector> 
#include <iostream>

// solve(i) = maximum score obtainable using pins from i to the end
                    //take      skip            take both
//transition = max(pin[i], pin[i + 1], pin[i] * pin[i + 1])

int solve_rec(std::vector<int>& pins, int n) {
    if(n >= pins.size()) {
        return 0; 
    }

    int skip = solve_rec(pins, n + 1); 
    int take = pins[n] + solve_rec(pins, n + 1); 

    int best = std::max(skip, take); 
    if(n < pins.size() - 1) {
        int take_both = pins[n] * pins[n + 1] + solve_rec(pins, n + 2); 
        best = std::max(best, take_both);
    }
    return best; 
}

int solve(const std::vector<int>& pins, std::vector<int>& memo, int n) {
    if(n >= pins.size()) {
        return 0; 
    }

    if(memo[n] != -1) {
        return memo[n]; 
    }

    int skip = solve(pins, memo, n + 1); 
    int take = pins[n] + solve(pins, memo, n + 1); 

    int best = std::max(skip, take); 
    if(n < pins.size() - 1) {
        int take_both = pins[n] * pins[n + 1] + solve(pins, memo, n + 2); 
        best = std::max(best, take_both);
    }
    return memo[n] = best; 
}

int solve_memo(const std::vector<int>& pins) {
    std::vector<int> memo(pins.size(), -1);
    return solve(pins, memo, 0); 
}

// dp[i] = best result using pins from i to the end
int solve_dp(const std::vector<int>& pins) {
    int n = pins.size(); 
    std::vector<int> dp(n + 2, 0); 

    dp[0] = 0;
    dp[1] = std::max(0, pins[0]);  

    for(int i = n - 1; i >= 0; --i) {
        int take = pins[i] + dp[i + 1]; 
        int skip = dp[i + 1]; 
        int best = std::max(take, skip); 
        if(i < n - 1) {
            int take_both = pins[i] * pins[i + 1] + dp[i + 2];
            best = std::max(best, take_both);
        }
        dp[i] = best;
    }
    return dp[0]; 
}



int main() {
    std::vector<int> pins = {
    -1, 1, 1, 1, 9, 9, 3, -3, -5, 2, 2
    };
    std::cout << solve_rec(pins, 0) << " " << solve_memo(pins) << " " << solve_dp(pins);  
}