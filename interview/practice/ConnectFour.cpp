
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <vector>
#include <assert.h>
#include <string>
#include <format>

enum class GameState: std::uint8_t {
    DRAW, 
    CUR_PLAYER_WIN,
    IN_PROCESS
};

enum class CellValue: std::uint8_t {
    EMPTY, 
    PLAYER_1,
    PLAYER_2
};

struct Board {
    const int cols{}; 
    const int rows{};
    const int connects{}; 
    const int total{}; 

    int filled{};

    std::vector<int> columns_counts;
    std::vector<std::vector<CellValue>> grid;

    Board(int c, int r, int cnct): cols(c), rows(r), connects(cnct), total(c * r),
        columns_counts(std::vector<int>(cols)), 
        grid(std::vector<std::vector<CellValue>>(rows, std::vector<CellValue>(cols, CellValue::EMPTY))) 
        {}

};


class Game {
    Board board; 
    constexpr static int deltas[4][2] = {
            {0, 1}, {1, 0}, {1, 1}, {-1, 1}
        };
    CellValue next_player_turn{CellValue::PLAYER_1}; 
    GameState game_state{GameState::IN_PROCESS};
     

public:
    Game(): Game(7, 6, 4){}
    explicit Game(int c, int r, int cnct): board(verify_dimension(c), verify_dimension(r), verify_connects(r, c, cnct)) {}

    GameState next_turn(int col) {
        if(game_state != GameState::IN_PROCESS) {
            throw std::invalid_argument("game already finished"); 
        }

        if(col < 0 || col >= board.cols) {
            throw std::invalid_argument(std::format("column {} should be between 0 and {}", col, board.cols - 1));
        }

        int& already_placed = board.columns_counts[col];
        if(already_placed == board.rows) {
            throw std::invalid_argument("this column already filled"); 
        }

        already_placed++;
        int row = board.rows - already_placed;
        board.grid[row][col] = next_player_turn; 
        board.filled++;

        GameState state = get_current_game_state(next_player_turn, row, col);
        if(state == GameState::IN_PROCESS) {
            update_player();
        }
        return game_state = state; 
    }

private:
    static int verify_dimension(int d) {
        if(d < 2) {
            throw std::invalid_argument("dimension should be larger then 1");
        }
        return d; 
    }

    static int verify_connects(int r,int c, int cnct) {
        int min_d = std::max(r, c); 
        if(cnct < 2 || cnct > min_d) {
            throw std::invalid_argument(std::format("you can't connect less then 2 and larger then {}", min_d));
        }
        return cnct; 
    }

    GameState get_current_game_state(CellValue cell_value, int row, int col) {
        if(current_player_win(cell_value, row, col)) {
            return GameState::CUR_PLAYER_WIN;
        }

        if(board.filled == board.total) {
            return GameState::DRAW; 
        }

        return GameState::IN_PROCESS; 
    }

    bool current_player_win(CellValue cell_value, int row, int col) {
        for(auto& delta : deltas) {
            int count = 
                count_in_direction(cell_value, row, col, delta[0], delta[1]) +
                count_in_direction(cell_value, row, col, -delta[0], -delta[1]) + 1; 

            if(count >= board.connects) {
                return true;
            }
        }
        return false; 
    }

    int count_in_direction(CellValue v, int row, int col, int dr, int dc) {
        int count = 0; 
        for(int i = 1; i <= board.connects - 1; i++) {
            int new_row = row + dr * i;
            int new_col = col + dc * i;
            if(!valid_cell(v, new_row, new_col)) {
                return count;
            }
            count++;
        }
        return count;
    }

    bool valid_cell(CellValue v, int row, int col) {
        return row >= 0 && col >= 0 && row < board.rows && col < board.cols && board.grid[row][col] == v; 
    }

    void update_player() {
        next_player_turn = next_player_turn == CellValue::PLAYER_1 ? CellValue::PLAYER_2 : CellValue::PLAYER_1; 
    }
};


int main() {

    //vertical win
    Game g; 
    try {
        g.next_turn(8);
    } catch(std::exception& e) {
        assert(e.what() == std::format("column {} should be between 0 and {}", 8, 6));
    }

    assert(g.next_turn(0) == GameState::IN_PROCESS); 
    g.next_turn(1);
    g.next_turn(0);
    g.next_turn(1);
    g.next_turn(0);
    g.next_turn(1);
    assert(g.next_turn(0) == GameState::CUR_PLAYER_WIN);
    try {
        g.next_turn(1);
    } catch(std::exception& e) {
        assert(e.what() == std::string("game already finished"));
    }

    //horizontal win
    Game g2; 
    assert(g2.next_turn(0) == GameState::IN_PROCESS); 
    g2.next_turn(0);
    g2.next_turn(1);
    g2.next_turn(1);
    g2.next_turn(2);
    g2.next_turn(2);
    assert(g2.next_turn(3) == GameState::CUR_PLAYER_WIN);

    //full columns exception
    Game g3; 
    assert(g3.next_turn(0) == GameState::IN_PROCESS); 
    g3.next_turn(0);
    g3.next_turn(0);
    g3.next_turn(0);
    g3.next_turn(0);
    g3.next_turn(0);
    try {g3.next_turn(0);
    } catch(std::exception& e) {
        assert(e.what() == std::string("this column already filled"));
    };

    //diagonal win 
    Game g4; 
    assert(g4.next_turn(0) == GameState::IN_PROCESS);
    g4.next_turn(1); 
    g4.next_turn(2); 
    g4.next_turn(3); 

    g4.next_turn(1); 
    g4.next_turn(2); 
    g4.next_turn(3); 

    g4.next_turn(1); 
    g4.next_turn(2); 
    g4.next_turn(3); 

    assert(g4.next_turn(3) == GameState::CUR_PLAYER_WIN);
}