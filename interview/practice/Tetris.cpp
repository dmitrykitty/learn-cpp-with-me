#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>
#include <set>

using namespace std; 

class Tetris {
    const int H; 
    const int W; 

    vector<string> board; 
    //top[c] represents row of last filled cell in columns c
    vector<int> top; 
    vector<int> row_filled; 

    int score{0}; 
    bool is_game_in_process{true}; 

public: 
    Tetris(int h, int w) : 
        H(validate_dim(h)), W(validate_dim(w)), board(H, string(W, '.')), top(W, H), row_filled(H,0) {}

    //figure representet by shifts from top most left corner
    //accepted only positive shifts
    //
    bool drop(const vector<pair<int, int>>& figure, int col) {
        if(!is_game_in_process) {
            throw domain_error("game over!"); 
        }

        if(col < 0 || col >= W) {
            throw invalid_argument("invalid columns"); 
        }

        if(figure.empty()) {
            throw invalid_argument("invalid shift"); 
        }
        int min_top_point = H; 

        for(const auto& [dr, dc] : figure) {
            if(dr < 0 || dc < 0) {
                throw invalid_argument("invalid shift"); 
            }

            if(col + dc >= W) {
                throw invalid_argument("invalid columns"); 
            }; 

            int cur_top_point = top[col + dc] - 1 - dr; 
            min_top_point = min(min_top_point, cur_top_point); 
        }

        if(min_top_point < 0) {
            return is_game_in_process = false;         
        }

        set<int> used_rows; 
        //coord of top left corner = min_top_point, col
        for(const auto& [dr, dc] : figure) {
            int filled_r = min_top_point + dr, filled_c = col + dc;
            used_rows.insert(filled_r); 
            row_filled[filled_r]++; 
            board[filled_r][filled_c] = '#'; 
            top[filled_c] = min(top[filled_c], filled_r); 
        }

        for(int row : used_rows) {
            if(row_filled[row] == W) {
                update_board(row);
                update_rows(row);  
                update_tops(row); 
                
                score++;  
            }
        }
        return true;
    }

    vector<string> rows() const {
        return board; 
    }

    bool is_game_over() const {
        return !is_game_in_process; 
    }

    int get_score() const {
        return score; 
    }

private: 
    static int validate_dim(int dim) {
        if(dim < 1) {
            throw invalid_argument("invalid_dimension"); 
        }
        return dim;
    }

    void update_board(int row) {
        board.erase(board.begin() + row); 
        board.insert(board.begin(), string(W, '.')); 
    }

    void update_tops(int row) {
        for(int i = 0; i < top.size(); ++i) {
            //removed line below
            if(row == top[i]++) {
                while(top[i] < H && board[top[i]][i] == '.') {
                    top[i]++; 
                }
            }
        }
    }

    void update_rows(int row) {
        row_filled.erase(row_filled.begin() + row); 
        row_filled.insert(row_filled.begin(), 0); 
    }
};

// I, horizontal: ####
const vector<pair<int, int>> I_H = {{0,0}, {0,1}, {0,2}, {0,3}};

// I, vertical
const vector<pair<int, int>> I_V = {{0,0}, {1,0}, {2,0}, {3,0}};

// O
// ##
// ##
const vector<pair<int, int>> O = {{0,0}, {0,1}, {1,0}, {1,1}};

// L
// #.
// #.
// ##
const vector<pair<int, int>> L = {{0,0}, {1,0}, {2,0}, {2,1}};

// J
// .#
// .#
// ##
const vector<pair<int, int>> J = {{0,1}, {1,1}, {2,0}, {2,1}};

// T
// ###
// .#.
const vector<pair<int, int>> T = {{0,0}, {0,1}, {0,2}, {1,1}};

// T
// .#.
// ###
const vector<pair<int, int>> T_REV = {{0,1}, {1,0}, {1,1}, {1,2}};

// S  ((0,0) is empty)
// .##
// ##.
const vector<pair<int, int>> S = {{0,1}, {0,2}, {1,0}, {1,1}};

// Z
// ##.
// .##
const vector<pair<int, int>> Z = {{0,0}, {0,1}, {1,1}, {1,2}};

// Γ  (non-flat bottom, the example from before)
// ##
// .#
// .#
const vector<pair<int, int>> GAMMA = {{0,0}, {0,1}, {1,1}, {2,1}};


void print_board(const vector<string>& board) {
    for(int i = 0; i < board.size(); ++i) {
        
        cout << board[i] << '\n'; 
    }
    cout << "-------------------\n";
}


template <typename E, typename F>
void expected_invalid(F f) {
    try { f();} catch(const E&) { return; }
    assert(false && "expected invalid"); 
}


int main() {
    vector<string> board = {".....", ".....", ".....", ".....", "....."}; 
    Tetris t1(5, 5);
    assert(board == t1.rows()); 

    print_board(t1.rows()); 

    assert(t1.drop(O, 0)); 
    vector<string> board_O = {".....", ".....", ".....", "##...", "##..."}; 
    assert(t1.rows() == board_O); 
    assert(0 == t1.get_score()); 
    print_board(t1.rows()); 

    t1.drop(T, 2); 
    print_board(t1.rows());
    vector<string> board_O_T = {".....", ".....", ".....", ".....", "##.#."}; 
    assert(t1.rows() == board_O_T); 
    assert(1 == t1.get_score()); 

    t1.drop(Z, 1); 
    print_board(t1.rows());
    vector<string> board_O_T_Z = {".....", ".....", ".##..", "..##.", "##.#."}; 
    assert(t1.rows() == board_O_T_Z); 

    t1.drop(I_H, 0);
    print_board(t1.rows());
    vector<string> board_O_T_Z_IH = {".....", "####.", ".##..", "..##.", "##.#."}; 
    assert(t1.rows() == board_O_T_Z_IH); 


    expected_invalid<invalid_argument>([&]{t1.drop({}, 1); }); 
    assert(t1.rows() == board_O_T_Z_IH);
    expected_invalid<invalid_argument>([&]{t1.drop({{{-1, 1}}}, 1); }); 
    assert(t1.rows() == board_O_T_Z_IH);
    expected_invalid<invalid_argument>([&]{t1.drop({{1, 1}}, -1); }); 
    assert(t1.rows() == board_O_T_Z_IH);
    expected_invalid<invalid_argument>([&]{t1.drop({{1, 1}}, 5); }); 
    assert(t1.rows() == board_O_T_Z_IH);

    assert(t1.drop(I_H, 1));
    print_board(t1.rows());
    vector<string> board_O_T_Z_IH_IH = {".####", "####.", ".##..", "..##.", "##.#."}; 
    assert(t1.rows() == board_O_T_Z_IH_IH); 

    assert(!t1.drop(I_H, 0)); 
    assert(t1.rows() == board_O_T_Z_IH_IH); 
    expected_invalid<domain_error>([&]{t1.drop(I_H, 0); }); 
    assert(t1.is_game_over()); 
    assert(t1.get_score() == 1); 
    assert(t1.rows() == board_O_T_Z_IH_IH); 

    cout << "\nt1 ok\n"; 
    
    expected_invalid<invalid_argument>([]{Tetris t2(-1, 1);});  
    expected_invalid<invalid_argument>([]{Tetris t2(1, -1);});  

    Tetris t2(5, 5); 
    vector<string> board_L = {".....", ".....", "..#..", "..#..", "..##."}; 
    t2.drop(L, 2); 
    print_board(t2.rows());
    assert(t2.rows() == board_L); 

    expected_invalid<invalid_argument>([&] { t2.drop(I_H, 3); }); 
    t2.drop(T, 2); 
    print_board(t2.rows());
    vector<string> board_L_T = {".....", "..###", "..##.", "..#..", "..##."}; 
    assert(t2.rows() == board_L_T);

    t2.drop(S, 0); 
    print_board(t2.rows());
    vector<string> board_L_T_S = {".....",".##..", "..##.", "..#..", "..##."}; 
    assert(t2.rows() == board_L_T_S);
    assert(t2.get_score() == 1); 

    assert(!t2.drop({S}, 1)); 
    assert(t2.is_game_over()); 
    expected_invalid<domain_error>([&] { t2.drop(L, 1); }); 


    //multiple rows filled
    Tetris t3(4, 4); 
    auto empty = t3.rows(); 
    t3.drop(O, 0);
    print_board(t3.rows());
    t3.drop(O, 2); 
    print_board(t3.rows()); 
    assert(empty == t3.rows()); 
    assert(t3.get_score() == 2); 

    t3.drop(T_REV, 1); 
    t3.drop(T, 1);
    vector<string> board_TREV_T = {".###", "..#.", "..#.", ".###"};  
    print_board(t3.rows()); 
    assert(t3.rows() == board_TREV_T); 
    assert(t3.get_score() == 2); 

    t3.drop(I_V, 0); 
    vector<string> board_TREV_T_IV = {"....", "....", "#.#.", "#.#."};  
    print_board(t3.rows());
    assert(t3.rows() == board_TREV_T_IV);
    assert(t3.get_score() == 4); 
    
}