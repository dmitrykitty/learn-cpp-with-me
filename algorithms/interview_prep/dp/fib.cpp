#include <iostream>
#include <vector>

//solve(n) = Fib_n
//transition: fib(n) = fib(n - 1) + fib(n - 2)
long long fib_rec(int n) {
    if(n <= 1) {
        return 1; 
    }
    return fib_rec(n - 1) + fib_rec(n -2);
}

long long fib_memo(int n, std::vector<long long>& memo) {
    if(n <= 1) {
        return 1; 
    }

    if(memo[n] != -1) {
        return memo[n];
    }

    return memo[n] = fib_memo(n - 1, memo) + fib_memo(n - 2, memo);
}

long long fib_dp(int n) {
    if(n <= 1) {
        return 1;
    }
    
    std::vector<long long> dp(n, 1); 
    for(int i = 2; i < n; ++i) {
        dp[i] = dp[i - 1] + dp[i - 2]; 
    }
    return dp[n];
}


int main() {

}