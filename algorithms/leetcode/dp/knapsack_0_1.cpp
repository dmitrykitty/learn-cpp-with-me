/*
The hiking backpack

You’re packing for a multi-day hike in the Tatras. Your backpack can carry at most W kilograms. At home you have n items. Item i weighs weight[i] kg and has a usefulness score of value[i]. There is only one of each item, so you either take it or leave it, and you can’t take half an item.

Choose a set of items whose total weight is at most W and whose total usefulness is as large as possible. Return that maximum total usefulness.

Constraints: 1 ≤ n ≤ 100, 1 ≤ W ≤ 10⁵, 1 ≤ weight[i] ≤ W, 1 ≤ value[i] ≤ 10⁹.
*/

#include <bits/stdc++.h>

using namespace std; 

// 0/1 Knapsack ("The hiking backpack") — all versions
// State: f(idx, W) = max usefulness using only items 0..idx with remaining capacity W
// Transition: f(idx, W) = max(not_take = f(idx-1, W),
//                             take     = value[idx] + f(idx-1, W - weight[idx])   if W >= weight[idx])
// Base: f(0, W) = W >= weight[0] ? value[0] : 0
// Answer: f(n-1, W)
 
#include <bits/stdc++.h>
using namespace std;
 
// ---------------------------------------------------------------
// 1. Plain recursion — O(2^n) time, O(n) stack
// ---------------------------------------------------------------
long long f_rec(int idx, int W, const vector<int>& weight, const vector<long long>& value) {
    if(idx == 0) {
        return W < weight[0] ? 0 : value[0];
    }
 
    long long not_take = 0 + f_rec(idx - 1, W, weight, value);
    long long take = INT_MIN; //because we want to find max -> MIN value won't be selected in case if this path impossible
    if(W >= weight[idx]) {
        take = value[idx] + f_rec(idx - 1, W - weight[idx], weight, value);
    }
    return max(not_take, take);
}
 
long long maxUsefulness_rec(int W, const vector<int>& weight, const vector<long long>& value) {
    int N = weight.size() - 1; //values avaluable
    return f_rec(N, W, weight, value);
}
 
// ---------------------------------------------------------------
// 2. Memoization — O(n*W) time, O(n*W) memo + O(n) stack
// ---------------------------------------------------------------
long long f_memo(int idx, int W, const vector<int>& weight, const vector<long long>& value, vector<vector<long long>>& memo) {
    if(idx == 0) {
        return W < weight[0] ? 0 : value[0];
    }
 
    if(memo[idx][W] != -1) {
        return memo[idx][W];
    }
 
    long long not_take = 0 + f_memo(idx - 1, W, weight, value, memo);
    long long take = INT_MIN; //because we want to find max -> MIN value won't be selected in case if this path impossible
    if(W >= weight[idx]) {
        take = value[idx] + f_memo(idx - 1, W - weight[idx], weight, value, memo);
    }
    return memo[idx][W] = max(not_take, take);
}
 
long long maxUsefulness_memo(int W, const vector<int>& weight, const vector<long long>& value) {
    int n = weight.size();  // FIX: size of memo = number of items (was N = size - 1 -> memo[N] out of bounds)
    vector<vector<long long>> memo(n, vector<long long>(W + 1, -1));
    return f_memo(n - 1, W, weight, value, memo);
}
 
// ---------------------------------------------------------------
// 3. Bottom-up 2D — O(n*W) time, O(n*W) space
//    Fill from the base (idx = 0) up to the answer (idx = n-1)
// ---------------------------------------------------------------
long long maxUsefulness_dp(int W, const vector<int>& weight, const vector<long long>& value) {
    int N = weight.size(); //values avaluable
    vector<vector<long long>> dp(N, vector<long long>(W + 1, 0));
 
    //fill only values where we actually can peak last thing
    for(int w = weight[0]; w <= W; ++w) {   // FIX: loop over weights up to W (was i < N), item 0 only
        dp[0][w] = value[0];                // FIX: value[0] (was value[i])
    }
 
    for(int idx = 1; idx < N; ++idx) {
        for(int cur_weight = 0; cur_weight <= W; ++cur_weight) {
            long long not_take = 0 + dp[idx - 1][cur_weight];
            long long take = INT_MIN; //because we want to find max -> MIN value won't be selected in case if this path impossible
            if(cur_weight >= weight[idx]) {
                take = value[idx] + dp[idx - 1][cur_weight - weight[idx]];
            }
            dp[idx][cur_weight] = max(not_take, take);
        }
    }
    return dp[N - 1][W];
}
 
// ---------------------------------------------------------------
// 4. Two rows — O(n*W) time, O(W) space
//    Row idx reads only row idx-1, so keep just prev and cur
// ---------------------------------------------------------------
long long maxUsefulness_two_rows(int W, const vector<int>& weight, const vector<long long>& value) {
    int N = weight.size(); //values avaluable
    vector<long long> prev(W + 1, 0), cur(W + 1, 0);
    // FIX: removed the leftover 2D dp table (it allocated the full n*W memory again)
 
    //fill only values where we actually can peak last thing
    for(int w = weight[0]; w <= W; ++w) {
        prev[w] = value[0];
    }
 
    for(int idx = 1; idx < N; ++idx) {
        for(int cur_weight = 0; cur_weight <= W; ++cur_weight) {
            long long not_take = 0 + prev[cur_weight];
            long long take = INT_MIN; //because we want to find max -> MIN value won't be selected in case if this path impossible
            if(cur_weight >= weight[idx]) {
                take = value[idx] + prev[cur_weight - weight[idx]];
            }
            cur[cur_weight] = max(not_take, take);
        }
        swap(prev, cur); // FIX (style): O(1) swap instead of O(W) copy
    }
    return prev[W]; // FIX: answer is in prev (was cur[W] -> returned 0 when n == 1)
}
 
// ---------------------------------------------------------------
// 5. One row — O(n*W) time, O(W) space
//    Weight loop goes BACKWARDS: cur[w - weight[idx]] must still hold
//    the previous row's value when we read it.
//    (Forwards would let item idx be taken again -> unbounded knapsack.)
// ---------------------------------------------------------------
long long maxUsefulness_one_row(int W, const vector<int>& weight, const vector<long long>& value) {
    int N = weight.size(); //values avaluable
    vector<long long> cur(W + 1, 0);
 
    //fill only values where we actually can peak last thing
    for(int w = weight[0]; w <= W; ++w) {
        cur[w] = value[0];
    }
 
    for(int idx = 1; idx < N; ++idx) {
        for(int cur_weight = W; cur_weight >= 0; --cur_weight) {
            long long not_take = 0 + cur[cur_weight]; //for now past value prov prev row is saved here
            long long take = INT_MIN; //because we want to find max -> MIN value won't be selected in case if this path impossible
            if(cur_weight >= weight[idx]) {
                take = value[idx] + cur[cur_weight - weight[idx]];
            }
            cur[cur_weight] = max(not_take, take);
        }
    }
    return cur[W];
}