/*
A triathlete trains for n days. Each day she does exactly one discipline: swimming (0), cycling (1) or running (2). Her coach has measured how much each session helps on each day: points[day][discipline]. The coach has one rule: she must not do the same discipline on two consecutive days.

Return the maximum total training benefit over all n days.

Constraints: 1 ≤ n ≤ 10⁵, 1 ≤ points[day][discipline] ≤ 100.
*/
#include <bits/stdc++.h>
#include <vector>

using namespace std; 

int f(int day, int forbidden, const vector<vector<int>>& points, vector<vector<int>>& memo) {
    if(day == 0) {
        int max_base = 0;
        for(int i = 0; i < 3; ++i) {
            if(i != forbidden){
                max_base = max(max_base, points[0][i]);
            }
        }
        return max_base;  
    }

    if(memo[day][forbidden] != -1) {
        return memo[day][forbidden];
    }

    int max_val = 0; 
    for(int i = 0; i < 3; i++) {
        if(i != forbidden) {
            max_val = max(f(day - 1, i, points, memo) + points[day][i], max_val);
        }
    }
    return memo[day][forbidden] = max_val; 
}

int ninjaTraining(int n, vector<vector<int>>& points) {
    int res = 0; 
    vector<vector<int>> memo(n, vector<int>(3, -1)); 

    for(int i = 0; i < 3; ++i) {
        int cur = points[n - 1][i]; 
        if(n > 1) {
            cur += f(n - 2, i, points, memo);
        }
        res = max(res, cur); 
    }

    return res; 
}


int ninjaTrainingDP(int n, vector<vector<int>>& points) {
    int res = 0; 
    vector<int> prev(3, 0);
    vector<int> next(3, 0);

    //fill base case
    for(int forbidden = 0; forbidden < 3; ++forbidden) {
        for(int i = 0; i < 3; ++i) {
            if(i != forbidden){
                prev[forbidden] = max(prev[forbidden], points[0][i]);
            }
        }
    }

    for(int day = 1; day < n - 1; ++day) {
        for(int forbidden = 0; forbidden < 3; ++forbidden) {
            for(int i = 0; i < 3; ++i) {
                if(i != forbidden){
                    next[forbidden] = max(prev[i] + points[day][i], next[forbidden]);
                }
            }
        }
        swap(next, prev); 
    }

    for(int i = 0; i < 3; ++i) {
        int cur = points[n - 1][i];
        if(n > 1) {
            cur += prev[i];
        }
        res = max(res, cur);
    }

    return res; 
}


