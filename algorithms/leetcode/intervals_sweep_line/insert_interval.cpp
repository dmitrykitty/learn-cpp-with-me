#include <bits/stdc++.h>

using namespace std; 

void add_next_interval(vector<vector<int>>& res, vector<int>& new_interval) {
        if(res.empty() || res.back()[1] < new_interval[0]){
            res.push_back(new_interval);
        } else {
            res.back()[1] = max(res.back()[1], new_interval[1]); 
        }
}

vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& new_interval) {
        vector<vector<int>> res; 

        int idx = 0; 
        while(idx < intervals.size() && intervals[idx][0] < new_interval[0]) {
            res.push_back(intervals[idx++]); 
        }

        add_next_interval(res, new_interval); 

        for(int i = idx; i < intervals.size(); ++i) {
            add_next_interval(res, intervals[i]); 
        }

        return res; 
}

    