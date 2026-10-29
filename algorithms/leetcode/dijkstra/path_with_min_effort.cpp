
#include <bits/stdc++.h>

using namespace std; 

const int DIRS[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; 

    bool valid_coord(int r, int c, int n, int m) {
        return r >= 0 && c >= 0 && r < n && c < m; 
    }

    int minimumEffortPath(vector<vector<int>>& heights) {
        int n = heights.size(); 
	    int m = heights[0].size(); 

        //n*m
        vector<vector<int>> diffs(n, vector<int>(m, INT_MAX)); 
        priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> heap; 

        heap.push({0, 0, 0}); 
        diffs[0][0] = 0; 

        //4 * n * m * log(n * m)
        while(!heap.empty()) {
            auto [cur_diff, rc, cc] = heap.top(); 
            heap.pop(); 

            if(cur_diff > diffs[rc][cc]) continue; 

            for(const auto& dir: DIRS) {
                int new_rc = rc + dir[0]; 
                int new_cc = cc + dir[1]; 

                if(!valid_coord(new_rc, new_cc, n, m)) continue;

                int new_diff = max(cur_diff, abs(heights[rc][cc] - heights[new_rc][new_cc]));

                if(new_diff < diffs[new_rc][new_cc]) {
                    diffs[new_rc][new_cc] = new_diff; 
                    heap.push({new_diff, new_rc, new_cc}); 
                }
            }
        } 
        return diffs[n - 1][m - 1]; 
    }