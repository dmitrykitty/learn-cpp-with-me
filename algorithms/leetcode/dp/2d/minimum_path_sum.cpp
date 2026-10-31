#include <bits/stdc++.h>

using namespace std; 

int minPathSum(vector<vector<int>>& grid) {
        int m = grid.size();
        int n = grid[0].size(); 

        vector<int> dp(n, 0); 

        for(int i = 0; i < m; ++i) {
            for(int j = 0; j < n; ++j) {
                if(i == 0 && j == 0) {
                    dp[j] = grid[i][j]; 
                    continue; 
                }

                int left = INT_MAX; 
                int top = INT_MAX;
                if(i > 0) top = dp[j];
                if(j > 0) left = dp[j-1];
                
                dp[j] = min(left, top) + grid[i][j];
                
            }
        }
        return dp[n-1];
    }