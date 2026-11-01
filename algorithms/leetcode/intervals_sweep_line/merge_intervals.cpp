#include <bits/stdc++.h>

using namespace std; 

vector<vector<int>> merge(vector<vector<int>>& intervals) {
        vector<vector<int>> res; 

        sort(intervals.begin(), intervals.end()); 

        for(const auto& interval: intervals) {
            if(res.empty() || res.back()[1] < interval[0]) {
                res.push_back(interval); 
            } else {
                res.back()[1] = max(interval[1], res.back()[1]); 
            }
        }
        return res;
    }