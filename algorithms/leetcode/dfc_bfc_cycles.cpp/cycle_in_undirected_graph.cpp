/*
LC1559
Wiktoria restores old mosaic floors in Kraków churches. The mosaic is a rectangular grid of small square tiles, and each tile has one colour, written as a lowercase letter. Before starting work, she wants to know whether the floor has a ring: a closed loop of tiles of one single colour, because rings are usually an intended decorative pattern and must be cleaned with a different method.

A ring is a path of four or more tiles that starts and ends at the same tile. From a tile you can step to one of the four tiles directly next to it (up, down, left, right, never diagonally), but only if that tile has the same colour as the current one. You must never step straight back onto the tile you just came from, so walking forward and back along a line doesn’t count. Rings of any shape count, and a plain 2 × 2 block of one colour is already a ring of four tiles. Different colours never mix in the same ring.

You’re given the mosaic grid as a vector<vector<char>>, with up to 500 rows and 500 columns. Return true if the floor contains at least one ring, and false otherwise.
*/

#include <vector>

const int DIRS[4][2] = {
{-1, 0}, {1, 0}, {0, -1}, {0, 1}
};
bool valid_cell(const std::vector<std::vector<char>>& grid, int next_rc, int next_cc, int prev_rc, int prev_cc) {
    //color shoould be the same and cell should't be out of boarder 
    return next_rc >= 0 && next_cc >= 0 && next_rc < grid.size() && next_cc < grid[0].size() && grid[prev_rc][prev_cc] == grid[next_rc][next_cc];
}

bool detect_cycle_with_dfs(const std::vector<std::vector<char>>& grid, std::vector<std::vector<bool>>& visited, int par_rc, int par_cc, int rc , int cc) {
    visited[rc][cc] = true; 

    for(const auto& dir: DIRS) {
        int next_rc = rc + dir[0];
        int next_cc = cc + dir[1]; 

        //skip because of parent 
        if(next_rc == par_rc && next_cc == par_cc) {
            continue;
        }

        //skip wrong color or out of bounds
        if(!valid_cell(grid, next_rc, next_cc, rc, cc)) {
            continue;
        }

        //came to the same cell from other direction -> cycle
        if(visited[next_rc][next_cc] || detect_cycle_with_dfs(grid, visited, rc, cc, next_rc, next_cc)) {
            return true;
        }
    }
    return false; 
}

bool has_ring(const std::vector<std::vector<char>>& grid) {
    int n = grid.size(); 
    int m = grid[0].size(); 
    std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false)); 

    for(int rc = 0; rc < n; ++rc) {
        for(int cc =0; cc < m; ++cc) {
            if(!visited[rc][cc] && detect_cycle_with_dfs(grid, visited, -1, -1, rc, cc)) {
                    return true; 
            }
        } 
    }
    return false; 
}