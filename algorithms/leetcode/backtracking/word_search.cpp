/*
Ola designs puzzle boards for a board-game café in Kazimierz. Her newest game is a grid of letter tiles laid out on a table. The players’ goal is to prove that a secret word is hidden in the grid. A word is hidden if you can spell it by starting on any tile and then moving, one letter at a time, to a tile directly to the left, right, above or below the current one. Diagonal moves are not allowed. A single tile may be used only once while spelling a given word, so you can’t step back onto a tile you have already used.

Before printing the boards, Ola wants a program to check that the secret word really is hidden in each one, otherwise the game would be unwinnable. The program only needs to answer yes or no.

You’re given a grid board of single English letters (upper- and lowercase both appear, and they count as different letters) with m rows and n columns, and a string word. Return true if the word can be spelled in the grid under these rules, and false otherwise. The grid has at most 6 rows and 6 columns, and the word has at most 15 letters.
*/


#include <vector>
#include <iostream>
#include <utility>

const std::vector<std::pair<int, int>> DIRS = {
    {-1, 0}, {0, -1},{0, 1}, {1, 0}
};

bool valid_tile(int new_row, int new_col, int n, int m, std::vector<std::vector<bool>>& used) {
    return new_col >= 0 && new_row >= 0 && new_row < n && new_col < m && !used[new_row][new_col]; 
}

bool check_the_path(
    const std::vector<std::vector<char>>& board, 
    std::vector<std::vector<bool>>& used, 
    const std::string& word, 
    int row, int col, int letter_number
) {
    if(board[row][col] != word[letter_number]) {
        return false;
    }


    if(letter_number == word.size() - 1) {
        return true;
    }

    used[row][col] = true;
    for(auto& dir: DIRS) {
        int new_row = row + dir.first;
        int new_col = col + dir.second;
        if(valid_tile(new_row, new_col, board.size(), board[0].size(), used) &&
            check_the_path(board, used, word, new_row, new_col, letter_number + 1)) {
            return true;
        }
    }
    used[row][col] = false;
    return false; 
}

bool word_exists(const std::vector<std::vector<char>>& board, const std::string& word) {
    int n = board.size();
    int m = board[0].size();

    std::vector<std::vector<bool>> used(n, std::vector<bool>(m));

    for(int row = 0; row < n; row++) {
        for(int col = 0; col < m; col++) {
            if(check_the_path(board, used, word, row, col, 0)) {
                return true;
            }
        }
    }
    return false; 
}