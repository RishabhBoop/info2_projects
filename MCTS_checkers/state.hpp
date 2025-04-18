/**
 * @file state.hpp
 * @brief Defines the classes and constants for representing the state of a checkers game.
 * 
 * This includes classes for Moves, Pieces, the Board, the overall GameState, and MCTS nodes.
 * It also defines constants for players, colors, and OS detection.
 */
#ifndef STATE_HPP
#define STATE_HPP
#include <iostream>
#include <vector>
#include <array> // Add include for std::array

/** @def OS_LINUX
 *  @brief Macro defined as 1 if compiling on Linux (GCC), 0 otherwise (assuming Windows). Used for OS-specific commands like clearing the screen.
 */
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
/** @def RESET
 *  @brief Resets terminal text formatting to default.
 */
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

#define BGBLACK "\033[40m"    /* Black */
#define BGRED "\033[41m"      /* Red */
#define BGGREEN "\033[42m"    /* Green */
#define BGYELLOW "\033[43m"   /* Yellow */
#define BGBLUE "\033[44m"     /* Blue */
#define BGMAGENTA "\033[45m"  /* Magenta */
#define BGCYAN "\033[46m"     /* Cyan */
#define BGWHITE "\033[47m"    /* White */
/** @def DEFAULT_BG
 *  @brief Default terminal background color.
 */
#define DEFAULT_BG "\033[49m" /* Default Background */

/** @def PLAYER1_COLOR
 *  @brief ANSI color code for Player 1's pieces.
 */
#define PLAYER1_COLOR BOLDMAGENTA
/** @def PLAYER2_COLOR
 *  @brief ANSI color code for Player 2's pieces.
 */
#define PLAYER2_COLOR BOLDCYAN
/** @def EMPTY_FIELD_COLOR
 *  @brief ANSI color code for empty fields (uses default background).
 */
#define EMPTY_FIELD_COLOR DEFAULT_BG

/** @def PLAYER1_COLOR_BG
 *  @brief ANSI background color code for Player 1 when selected.
 */
#define PLAYER1_COLOR_BG BGMAGENTA
/** @def PLAYER2_COLOR_BG
 *  @brief ANSI background color code for Player 2 when selected.
 */
#define PLAYER2_COLOR_BG BGCYAN
/** @def SELECTED_FIELD_COLOR
 *  @brief ANSI background color code for a selected empty field.
 */
#define SELECTED_FIELD_COLOR BGWHITE
/** @def NOT_SELECTED_FIELD_COLOR
 *  @brief ANSI background color code for a non-selected field (default background).
 */
#define NOT_SELECTED_FIELD_COLOR DEFAULT_BG

/** @def PLAYER1
 *  @brief Integer identifier for Player 1.
 */
#define PLAYER1 1
/** @def PLAYER2
 *  @brief Integer identifier for Player 2.
 */
#define PLAYER2 2
/** @def NOPLAYER
 *  @brief Integer identifier for an empty field (no player).
 */
#define NOPLAYER 0

#endif

using namespace std;

// forward declaration
class Move;
class Board;
class GameState;
class MCTS_leaf;
class Piece;

/**
 * @class Move
 * @brief Represents a single move in the checkers game.
 * 
 * Stores the source and destination coordinates, whether it's a jump,
 * and the coordinates of the jumped piece if applicable.
 */
class Move
{
private:
    int src_x;    /**< Source column coordinate (0-7). */
    int src_y;    /**< Source row coordinate (0-7). */
    int dest_x;   /**< Destination column coordinate (0-7). */
    int dest_y;   /**< Destination row coordinate (0-7). */
    bool jump;    /**< True if the move is a jump, false otherwise. */
    int enemy_x;  /**< Column coordinate of the jumped piece (-1 if not a jump). */
    int enemy_y;  /**< Row coordinate of the jumped piece (-1 if not a jump). */

public:
    /**
     * @brief Constructs a Move object.
     * @param src_x Source column.
     * @param src_y Source row.
     * @param dest_x Destination column.
     * @param dest_y Destination row.
     * @param jump True if it's a jump.
     * @param enemy_x Column of the jumped piece.
     * @param enemy_y Row of the jumped piece.
     */
    Move(int src_x, int src_y, int dest_x, int dest_y, bool jump, int enemy_x, int enemy_y) : src_x(src_x), src_y(src_y), dest_x(dest_x), dest_y(dest_y), jump(jump), enemy_x(enemy_x), enemy_y(enemy_y) {}
    
    /**
     * @brief Prints the move details to the console.
     */
    void print_move();

    /**
     * @brief Gets whether the move is a jump.
     * @return True if the move is a jump, false otherwise.
     */
    bool get_jump_type() { return jump; }

    /**
     * @brief Performs the move on the given board.
     * @param b The board to perform the move on.
     * @param mv The move to perform.
     */
    void perform_move(Board b, Move mv);
};

/**
 * @class Piece
 * @brief Represents a single checker piece on the board.
 */
class Piece
{
private:
    int player_id;        /**< ID of the player owning the piece (PLAYER1, PLAYER2, or NOPLAYER). */
    int x;                /**< Column number (left to right, 0-7). */
    int y;                /**< Row number (top to bottom, 0-7). */
    bool is_king;         /**< True if the piece is a king, false otherwise. */
    const char *color;    /**< ANSI color code for displaying the piece. */
    const char *selected; /**< ANSI background color code when the piece is selected. */

public:
    /**
     * @brief Constructs a Piece object.
     * @param p_id Player ID (default NOPLAYER).
     * @param x Initial column (default -1).
     * @param y Initial row (default -1).
     * @param king True if the piece starts as a king (default false).
     */
    Piece(int p_id = 0, int x = -1, int y = -1, bool king = false);

    /** @brief Gets the player ID of the piece. @return Player ID. */
    inline int get_id() { return player_id; }
    /** @brief Gets the column coordinate of the piece. @return Column (0-7). */
    inline int get_x() { return x; }
    /** @brief Gets the row coordinate of the piece. @return Row (0-7). */
    inline int get_y() { return y; }
    /** @brief Checks if the piece is a king. @return True if king, false otherwise. */
    inline bool get_king() { return is_king; }
    /** @brief Gets the ANSI color code of the piece. @return Color code string. */
    inline const char *get_color() { return color; }
    /** @brief Gets the ANSI background color code when selected. @return Background color code string. */
    inline const char *get_selected() { return selected; }

    /** @brief Sets the player ID. @param id New player ID. */
    void set_id(int id) { player_id = id; }
    /** @brief Sets the column coordinate. @param x_in New column. */
    void set_x(int x_in) { x = x_in; }
    /** @brief Sets the row coordinate. @param y_in New row. */
    void set_y(int y_in) { y = y_in; }
    /** @brief Sets the king status. @param choice True to make king, false otherwise. */
    void set_king(bool choice) { is_king = choice; }
    /** @brief Sets the display color. @param col New ANSI color code. */
    void set_color(const char *col) { color = col; }
    /** @brief Sets the selected background color. @param sel New ANSI background color code. */
    void set_selected(const char *sel) { selected = sel; }

    /**
     * @brief Sets the piece's appearance to selected.
     */
    void select();
    /**
     * @brief Resets the piece's appearance to deselected.
     */
    void deselect();
    /**
     * @brief Checks if the piece represents an empty square.
     * @return True if the player ID is NOPLAYER, false otherwise.
     */
    bool is_empty();
    /**
     * @brief Prints detailed information about the piece to the console.
     */
    void print_piece_info();
    /**
     * @brief Checks if given coordinates are within the board boundaries (0-7).
     * @param y Row coordinate.
     * @param x Column coordinate.
     * @return True if coordinates are in bounds, false otherwise.
     */
    bool in_bounds(int y, int x);
};

/**
 * @class Board
 * @brief Represents the 8x8 checkerboard.
 * 
 * Contains the arrangement of pieces and counts of pieces for each player.
 */
class Board
{
private:
    array<array<Piece, 8>, 8> board; /**< 2D array representing the board state. */
    int num_player_pieces[2];        /**< Stores the number of pieces for [Player1, Player2]. */
public:
    /**
     * @brief Constructs a Board object from a 2D array of Pieces.
     * Initializes the board and counts the pieces for each player.
     * @param arr_p An 8x8 std::array of Piece objects.
     */
    Board(array<array<Piece, 8>, 8> arr_p);

    /**
     * @brief Gets the Piece at the specified coordinates.
     * @param y Row coordinate (0-7).
     * @param x Column coordinate (0-7).
     * @return A copy of the Piece at the given location.
     */
    Piece get_Piece(int y, int x) const { return board[y][x]; };

    /**
     * @brief Prints the current state of the board to the console with colors.
     */
    void print_Board();
    /**
     * @brief Clears the console screen (OS-dependent).
     */
    void clear_screen();
    /**
     * @brief Sets the Piece at the specified coordinates.
     * @param y Row coordinate (0-7).
     * @param x Column coordinate (0-7).
     * @param piece The Piece object to place.
     */
    void set_piece(int y, int x, Piece piece);
    /**
     * @brief Sets the number of pieces for a given player.
     * @param player Player ID (PLAYER1 or PLAYER2).
     * @param new_num The new count of pieces.
     */
    void set_num_players(int player, int new_num) { num_player_pieces[player - 1] = new_num; };
    /**
     * @brief Gets the number of pieces for a given player.
     * @param player Player ID (PLAYER1 or PLAYER2).
     * @return The current count of pieces for that player.
     */
    int get_num_players(int player) { return num_player_pieces[player - 1]; };
};

/**
 * @class GameState
 * @brief Represents the complete state of the game at a point in time.
 * 
 * Includes the board, the current player, and a list of possible moves from this state.
 */
class GameState
{
private:
    Board board;                 /**< The current board configuration. */
    int current_player;          /**< ID of the player whose turn it is (PLAYER1 or PLAYER2). */
    vector<Move> possible_moves; /**< List of all valid moves for the current player from this state. */
public:
    /**
     * @brief Constructs a GameState object.
     * @param b The initial Board state.
     * @param curr_p The player whose turn it is.
     * @param v_movs An optional initial list of possible moves.
     */
    GameState(Board b, int curr_p, vector<Move> v_movs = {}) : board(b), current_player(curr_p), possible_moves(v_movs) {};

    /**
     * @brief Creates a deep copy of the current GameState.
     * @return A new GameState object identical to this one.
     */
    GameState clone() const;
    /**
     * @brief Determines if the current state is a terminal state (win/loss).
     * @return PLAYER1 if Player 1 wins, PLAYER2 if Player 2 wins, NOPLAYER otherwise.
     */
    int TerminalState();
    /**
     * @brief Generates and stores all possible moves for the piece at the given coordinates.
     * Considers both normal moves and jumps. If jumps are possible, only jumps are stored.
     * @param from_x Column coordinate of the piece to move.
     * @param from_y Row coordinate of the piece to move.
     */
    void list_possible_moves(int from_x, int from_y);
    /**
     * @brief Prints all currently stored possible moves to the console.
     */
    void print_all_moves();
    /**
     * @brief Gets the Board object associated with this GameState.
     * @return The Board object.
     */
    Board get_board() { return board; };
};

/**
 * @class MCTS_leaf
 * @brief Represents a node in the Monte Carlo Tree Search (MCTS) tree.
 * 
 * Stores game state, statistics (wins/total games), and tree structure (parent/children).
 */
class MCTS_leaf
{
private:
    GameState state;            /**< The GameState represented by this node. */
    Move move;                  /**< The move that led to this GameState from the parent node. */
    int wins;                   /**< Number of simulated game wins passing through this node. */
    int total_games;            /**< Total number of simulated games passing through this node. */
    bool is_computer;           /**< Flag indicating if the player to move in this state is the computer. */
    bool is_terminal;           /**< Flag indicating if this node represents a terminal game state. */
    /* Tree Structure */
    MCTS_leaf *parent;          /**< Pointer to the parent node in the MCTS tree (nullptr for root). */
    vector<MCTS_leaf *> children; /**< Vector of pointers to child nodes. */

public:
    /**
     * @brief Constructs an MCTS_leaf node.
     * @param s The GameState for this node.
     * @param mv The Move that led to this state.
     * @param p Pointer to the parent node (default nullptr).
     * @param c Vector of child node pointers (default empty).
     * @param w Initial win count (default 0).
     * @param tg Initial total game count (default 0).
     * @param ic Is the current player the computer? (default true).
     * @param it Is this a terminal state? (default false).
     */
    MCTS_leaf(GameState s, Move mv, MCTS_leaf *p = nullptr, vector<MCTS_leaf *> c = {}, int w = 0, int tg = 0, bool ic = true, bool it = false) : state(s), move(mv), wins(w), total_games(tg), is_computer(ic), is_terminal(it), parent(p), children(c) {};
};

#endif
