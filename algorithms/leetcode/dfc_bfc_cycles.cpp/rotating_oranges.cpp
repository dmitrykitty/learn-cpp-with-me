/*
Tomasz manages a small fruit warehouse near Kraków. The oranges are stored on one large shelf divided into square slots, so the shelf is a rectangular grid with m rows and n columns. Each slot is in one of three states: empty (0), holding a fresh orange (1), or holding a rotten orange (2).

Rot spreads in a regular way. Every minute, each fresh orange that is directly next to a rotten one (up, down, left or right, not diagonally) becomes rotten. All the oranges that rot in the same minute do so simultaneously, so an orange that rots during minute 3 starts infecting its neighbours only in minute 4. Rot cannot jump across an empty slot, because there is no orange there to carry it.

Tomasz wants to know how many minutes must pass before no fresh orange is left on the shelf. If some fresh orange is cut off from every rotten one, for example because empty slots surround it, it will never rot, and then he wants the answer to say so.

You’re given the shelf as grid, a vector<vector<int>> with values 0, 1 or 2. Return the minimum number of minutes until no fresh orange remains, or -1 if that never happens. The shelf is at most 10 × 10.
*/

#include <bits/stdc++.h>

const int DIRS[4][2] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}
};

bool can_spread_to_this_cell(const std::vector<std::vector<int>>& grid, int m, int n, int new_row, int new_col) {
    return new_row >= 0 && new_col >= 0 && new_row < m && new_col < n && grid[new_row][new_col] == 1; 
}

int minutes_until_all_rotten(std::vector<std::vector<int>> grid) {
    int m = grid.size();
    int n = grid[0].size(); 

    std::deque<std::pair<int, int>> rotten; 
    int fresh_oranges = 0;
    
    for(int row = 0; row < m; ++row) {
        for(int col = 0; col < n; ++col) {
            if(grid[row][col] == 1) {
                fresh_oranges++;
            } else if(grid[row][col] == 2) {
                rotten.push_front({row, col});
            }
        }
    }

    if(fresh_oranges == 0) {
        return 0;
    }
    if(rotten.empty()) {
        return -1;
    }

    int minutes_passed = 0;

    while(!rotten.empty()) {
        int sz = rotten.size(); 
        for(int i = 0; i < sz; i++) {
            auto cur = rotten.back(); 
            rotten.pop_back(); 
            for(const auto& dir: DIRS) {
                int new_row = cur.first + dir[0];
                int new_col = cur.second + dir[1];

                if(can_spread_to_this_cell(grid, m, n, new_row, new_col)) {
                    fresh_oranges--;
                    grid[new_row][new_col] = 2; 
                    rotten.push_front({new_row, new_col});
                }
            }
        }
        minutes_passed++;
        if(fresh_oranges == 0) {
            return minutes_passed;
        }
    }

    return -1;
}