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




// Ninja's Training — "what do I do today" formulation
//
// State:      f(day, today) = max total points for days 0..day,
//                             if on day `day` the activity is `today`
// Transition: f(day, today) = points[day][today]
//                             + max over yesterday != today of f(day - 1, yesterday)
// Base:       f(0, today)   = points[0][today]        (first day: anything is allowed)
// Answer:     max over today of f(n - 1, today)       (last day: any activity)
//
// The rule "not the same activity two days in a row" is checked on the LATER day
// of each pair: today looks back at yesterday and skips yesterday == today.

#include <bits/stdc++.h>
using namespace std;

const int ACTIVITIES = 3;

// ---------------------------------------------------------------
// 1. Plain recursion — O(3^n) time, O(n) stack
// ---------------------------------------------------------------
int f_rec(int day, int today, const vector<vector<int>>& points) {
    if (day == 0) {
        return points[0][today];                 // first day: no yesterday
    }

    int best_yesterday = 0;
    for (int yesterday = 0; yesterday < ACTIVITIES; ++yesterday) {
        if (yesterday == today) continue;        // same activity two days in a row
        best_yesterday = max(best_yesterday, f_rec(day - 1, yesterday, points));
    }
    return points[day][today] + best_yesterday;
}

int ninjaTraining_rec(int n, vector<vector<int>>& points) {
    int res = 0;
    for (int today = 0; today < ACTIVITIES; ++today) {   // last day: anything
        res = max(res, f_rec(n - 1, today, points));
    }
    return res;
}

// ---------------------------------------------------------------
// 2. Memoization — O(n * 3 * 3) = O(n) time, O(n) memo + O(n) stack
//    memo has exactly the shape of the state: [day][today]
// ---------------------------------------------------------------
int f_memo(int day, int today, const vector<vector<int>>& points, vector<vector<int>>& memo) {
    if (day == 0) {
        return points[0][today];
    }
    if (memo[day][today] != -1) {                // -1 is safe: all answers are >= 0
        return memo[day][today];
    }

    int best_yesterday = 0;
    for (int yesterday = 0; yesterday < ACTIVITIES; ++yesterday) {
        if (yesterday == today) continue;
        best_yesterday = max(best_yesterday, f_memo(day - 1, yesterday, points, memo));
    }
    return memo[day][today] = points[day][today] + best_yesterday;
}

int ninjaTraining_memo(int n, vector<vector<int>>& points) {
    vector<vector<int>> memo(n, vector<int>(ACTIVITIES, -1));
    int res = 0;
    for (int today = 0; today < ACTIVITIES; ++today) {
        res = max(res, f_memo(n - 1, today, points, memo));
    }
    return res;
}

// ---------------------------------------------------------------
// 3. Bottom-up, full table — O(n) time, O(n) space
//    dp[day][today] = f(day, today); fill from the base (day 0) forward,
//    because each row reads only the row above it.
//    The table covers ALL days 0..n-1, so the answer is just the max of the last row.
// ---------------------------------------------------------------
int ninjaTraining_dp(int n, vector<vector<int>>& points) {
    vector<vector<int>> dp(n, vector<int>(ACTIVITIES, 0));

    // base: f(0, today) = points[0][today]
    for (int today = 0; today < ACTIVITIES; ++today) {
        dp[0][today] = points[0][today];
    }

    for (int day = 1; day < n; ++day) {
        for (int today = 0; today < ACTIVITIES; ++today) {          // which cell
            int best_yesterday = 0;
            for (int yesterday = 0; yesterday < ACTIVITIES; ++yesterday) {  // choice inside the cell
                if (yesterday == today) continue;
                best_yesterday = max(best_yesterday, dp[day - 1][yesterday]);
            }
            dp[day][today] = points[day][today] + best_yesterday;
        }
    }

    return *max_element(dp[n - 1].begin(), dp[n - 1].end());
}

// ---------------------------------------------------------------
// 4. Two rows — O(n) time, O(1) space
//    Row `day` reads only row `day - 1`, so keep just prev and cur.
//    cur is fully overwritten for every `today`, so no stale values survive.
// ---------------------------------------------------------------
int ninjaTraining_two_rows(int n, vector<vector<int>>& points) {
    vector<int> prev(points[0].begin(), points[0].end());   // base row = day 0
    vector<int> cur(ACTIVITIES, 0);

    for (int day = 1; day < n; ++day) {
        for (int today = 0; today < ACTIVITIES; ++today) {
            int best_yesterday = 0;
            for (int yesterday = 0; yesterday < ACTIVITIES; ++yesterday) {
                if (yesterday == today) continue;
                best_yesterday = max(best_yesterday, prev[yesterday]);
            }
            cur[today] = points[day][today] + best_yesterday;
        }
        swap(prev, cur);                          // after the swap, prev = current day
    }

    return *max_element(prev.begin(), prev.end()); // prev holds the last day (also when n == 1)
}

// ---------------------------------------------------------------
// Tests
// ---------------------------------------------------------------
int main() {
    struct Test { vector<vector<int>> points; int expected; };
    vector<Test> tests = {
        {{{1, 2, 5}, {3, 1, 1}, {3, 3, 3}}, 11},
        {{{10, 40, 70}, {20, 50, 80}, {30, 60, 90}}, 210},
        {{{10, 50, 1}, {5, 100, 11}}, 110},
        {{{7, 3, 9}}, 9},                       // n = 1
        {{{1, 1, 1}, {1, 1, 1}}, 2},            // ties
    };

    bool all_ok = true;
    for (size_t t = 0; t < tests.size(); ++t) {
        auto pts = tests[t].points;
        int n = pts.size();
        int r[4] = {
            ninjaTraining_rec(n, pts),
            ninjaTraining_memo(n, pts),
            ninjaTraining_dp(n, pts),
            ninjaTraining_two_rows(n, pts),
        };
        bool ok = true;
        for (int x : r) ok &= (x == tests[t].expected);
        all_ok &= ok;
        printf("test %zu: expected %d | rec %d memo %d dp %d two_rows %d  %s\n",
               t, tests[t].expected, r[0], r[1], r[2], r[3], ok ? "OK" : "FAIL");
    }
    puts(all_ok ? "ALL OK" : "SOME FAILED");
}


