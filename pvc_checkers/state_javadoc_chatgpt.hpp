#ifndef STATE_HPP
#define STATE_HPP
#include <iostream>
#include <vector>
#include <array> // Add include for std::array

#ifdef __GNUC__ 
#define OS_LINUX 1
#elif defined(_WIN64)
#define OS_LINUX 0
#endif

/*
https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
https://en.wikipedia.org/wiki/ANSI_escape_code#Colors:~:text=bracketed%20paste%20mode.-,Select%20Graphic%20Rendition%20parameters,-%5Bedit%5D
https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
*/

#ifndef COLORS
#define RESET "\033[0m"          /* Reset */
#define BLACK "\033[30m"         /* Black */
#define RED "\033[31m"           /* Red */
#define GREEN "\033[32m"         /* Green */
#define YELLOW "\033[33m"        /* Yellow */
#define BLUE "\033[34m"          /* Blue */
#define MAGENTA "\033[35m"       /* Magenta */
#define CYAN "\033[36m"          /* Cyan */
#define WHITE "\033[37m"         /* White */
#define BOLDBLACK "\033[1;30m"   /* Bold Black */
#define BOLDRED "\033[1;31m"     /* Bold Red */
#define BOLDGREEN "\033[1;32m"   /* Bold Green */
#define BOLDYELLOW "\033[1;33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1;34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1;35m" /* Bold Magenta */
#define BOLDCYAN "\033[1;36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1;37m"   /* Bold White */

#define BGBLACK "\033[40m"   /* Black */
#define BGRED "\033[41m"     /* Red */
#define BGGREEN "\033[42m"   /* Green */
#define BGYELLOW "\033[43m"  /* Yellow */
#define BGBLUE "\033[44m"    /* Blue */
#define BGMAGENTA "\033[45m" /* Magenta */
#define BGCYAN "\033[46m"    /* Cyan */
#define BGWHITE "\033[47m"   /* White */
#define DEFAULT_BG "\033[49m" /* Default Background */

#define PLAYER1_COLOR BOLDMAGENTA
#define PLAYER2_COLOR BOLDCYAN
#define EMPTY_FIELD_COLOR DEFAULT_BG

#define PLAYER1_COLOR_BG BGMAGENTA
#define PLAYER2_COLOR_BG BGCYAN
#define SELECTED_FIELD_COLOR BGWHITE
#define NOT_SELECTED_FIELD_COLOR DEFAULT_BG

#define PLAYER1 1
#define PLAYER2 2
#define NOPLAYER 0

#endif

using namespace std;

/**
 * @class Piece
 * @brief TODO: Describe the Piece class.
 */
class Piece {
private:
    int player_id;
    int x; // coloumn number; left to right 0-7
    int y; // row number; top to bottom 0-7
    bool is_king;
    const char *color;
    const char *selected;
public:
    Piece(int p_id=0, int x=-1, int y=-1, bool king=false) : 
        player_id(p_id), x(x), y(y), is_king(king) {}
    int get_id() { return player_id; }
    int get_x() { return x; }
    int get_y() { return y; }
    bool get_king() { return is_king; }
    const char* get_color() { return color; }
    const char* get_selected() { return selected; }
    void set_id(int id) { player_id = id; }
    void set_x(int x_in) { x = x_in; }
    void set_y(int y_in) { y = y_in; }
    void set_king(bool choice) { is_king = choice; }
    void set_color(const char *col) { color = col; }
    void set_selected(const char *sel) { selected = sel; }
    
    void select();
    void deselect();
    bool is_empty() { player_id == 0 ? true : false; };
    void print_piece_info();
    bool in_bounds(int y, int x);
};

/**
 * @class Board
 * @brief TODO: Describe the Board class.
 */
class Board {
private:
    array<array<Piece, 8>, 8> board;
    int num_player_pieces[2]; // Number of pieces for each player
public:
    Board(array<array<Piece, 8>, 8> arr_p) :
        board(arr_p){}
    Piece get_Piece(int y, int x) const {return board[y][x];};
    void print_Board();
    void clear_screen();
    void set_piece(int y, int x, Piece piece);
    void move(int src_y, int src_x, int dest_y, int dest_x, bool jump, int enemy_y, int enemy_x);
    void set_num_players(int player, int new_num) {num_player_pieces[player-1] = new_num;};
    int get_num_players(int player) {return num_player_pieces[player-1];};
    // int get_num_rows() {return board.size();};
    // int get_num_cols() {return board[0].size();};
};


/**
 * @class GameState
 * @brief TODO: Describe the GameState class.
 */
class GameState {
private:
    Board board;
    int current_player;
    vector<Move> possible_moves; // list of all possible moves
public:
    GameState(Board b, int curr_p, vector<Move> v_movs = {}) : 
        board(b), current_player(curr_p), possible_moves(v_movs) {};

    GameState clone() const; // deep copying of GameState
    int TerminalState();
    void list_possible_moves(int from_x, int from_y);
    void print_all_moves();
};


/**
 * @class MCTS_leaf
 * @brief TODO: Describe the MCTS_leaf class.
 */
class MCTS_leaf {
private: 
    GameState state; // current state of board
    Move move; // move that lead to this state
    int wins; // how many wins from this state
    int total_games; // how many games played in total
    /* Tree Structure */
    MCTS_leaf* parent;
    vector<MCTS_leaf*> children;
    MCTS_leaf(GameState s);
};

/**
 * @class Move
 * @brief TODO: Describe the Move class.
 */
class Move {
    private:
        int src_x;
        int src_y;
        int dest_x;
        int dest_y;
        bool jump;
        int enemy_x;
        int enemy_y;
    public:
        Move(int src_x, int src_y, int dest_x, int dest_y, bool jump, int enemy_x, int enemy_y) :
            src_x(src_x), src_y(src_y), dest_x(dest_x), dest_y(dest_y), jump(jump), enemy_x(enemy_x), enemy_y(enemy_y) {}
        void print_move();
        bool get_jump_type() {return jump;}
        void perform_move(Board b, Move mv);
    };

#endif
