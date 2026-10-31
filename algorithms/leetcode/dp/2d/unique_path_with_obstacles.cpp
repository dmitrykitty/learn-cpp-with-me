#include <bits/stdc++.h>

using namespace std; 

int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        int n = obstacleGrid.size(), m = obstacleGrid[0].size();

        if(obstacleGrid[0][0] == 1 || obstacleGrid[n-1][m-1] == 1) {
            return 0; 
        }
        
        vector<int> prev(m, 0);

        for(int i = 0; i < n; ++i) {
            vector<int> cur(m, 0);
            for(int j = 0; j < m; j++) {
                if(obstacleGrid[i][j] == 1) {
                    cur[j] = 0; 
                    continue;
                }
                if(i == 0 && j == 0) {
                    cur[0] = 1; 
                    continue;
                }

                int left = 0; 
                int top = 0; 

                if(j > 0) left = cur[j-1];
                if(i > 0) top = prev[j];

                cur[j] = left + top; 
            }
            swap(cur, prev);
        }

        return prev[m-1];
    }