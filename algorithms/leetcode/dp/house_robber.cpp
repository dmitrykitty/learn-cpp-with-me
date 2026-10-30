#include <bits/stdc++.h>

using namespace std; 

int f(const vector<int>& coins, vector<int>& memo, int amount){ 
    if(amount == 0) {
        return 0; 
    }

    int res = INT_MAX / 2; 
    for(int i = 0; i < coins.size(); ++i) {
        if(coins[i] > amount) {
            continue;
        }

        int rest = amount - coins[i];
        int new_count = memo[rest] == -1 ? f(coins, memo, rest) : memo[rest];
        res = min(res, new_count);
    }
    
    return res + 1;
}

int coinChange(vector<int>& coins, int amount) {
    if(amount == 0) {
        return 0; 
    }

    vector<int> memo(amount + 1, -1);
    int res = f(coins, memo, amount); 
    return res >= INT_MAX / 2 ? -1 : res;    
}

