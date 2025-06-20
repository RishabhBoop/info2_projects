/**
 * @file classes.hpp
 * @brief Defines the classes and constants for representing the state of a checkers game.
 *
 * This includes classes for Moves, Pieces, the Board, the overall GameState, and MCTS nodes.
 * It also defines constants for players, colors, and OS detection.
 */
#ifndef CLASSES_HPP
#define CLASSES_HPP
#include "../includes.hpp"


using namespace std;

// parameter C in the UCB-formula
extern double C;

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
    int src_y;   /**< Source row coordinate (0-7). */
    int src_x;   /**< Source column coordinate (0-7). */
    int dest_y;  /**< Destination row coordinate (0-7). */
    int dest_x;  /**< Destination column coordinate (0-7). */
    bool jump;   /**< True if the move is a jump, false otherwise. */
    int enemy_y; /**< Row coordinate of the jumped piece (-1 if not a jump). */
    int enemy_x; /**< Column coordinate of the jumped piece (-1 if not a jump). */

public:
    /**
     * @brief Constructs a Move object.
     * @param src_y Source row (0-7). Defaults to -1.
     * @param src_x Source column (0-7). Defaults to -1.
     * @param dest_y Destination row (0-7). Defaults to -1.
     * @param dest_x Destination column (0-7). Defaults to -1.
     * @param jump True if it's a jump. Defaults to false.
     * @param enemy_y Row of the jumped piece (0-7). Defaults to -1.
     * @param enemy_x Column of the jumped piece (0-7). Defaults to -1.
     */
    Move(int src_y = -1, int src_x = -1, int dest_y = -1, int dest_x = -1, bool jump = false, int enemy_y = -1, int enemy_x = -1) : src_y(src_y), src_x(src_x), dest_y(dest_y), dest_x(dest_x), jump(jump), enemy_y(enemy_y), enemy_x(enemy_x) {}

    int get_src_y() const { return src_y; }     /**< @return Source row (0-7). */
    int get_src_x() const { return src_x; }     /**< @return Source column (0-7). */
    int get_dest_y() const { return dest_y; }   /**< @return Destination row (0-7). */
    int get_dest_x() const { return dest_x; }   /**< @return Destination column (0-7). */
    int get_enemy_y() const { return enemy_y; } /**< @return Row of the jumped piece (-1 if not a jump). */
    int get_enemy_x() const { return enemy_x; } /**< @return Column of the jumped piece (-1 if not a jump). */

    /**
     * @brief Prints the details of a single move to the console.
     * Shows source and destination coordinates, and jumped piece coordinates if applicable.
     */
    void print_move();

    /**
     * @brief Gets whether the move is a jump.
     * @return True if the move is a jump, false otherwise.
     */
    bool get_jump_type() const { return jump; }

    /**
     * @brief Performs the specified move on the given board state.
     *
     * Updates the piece positions on the board by swapping the source and destination pieces.
     * If the move is a jump, it removes the jumped enemy piece (sets it to NOPLAYER)
     * and updates the piece count for the opponent.
     *
     * @param b Pointer to the Board object to perform the move on.
     * @param mv The Move object describing the move to perform.
     */
    void perform_move(Board *b, Move mv);

    string get_move_info();
};

/**
 * @class Piece
 * @brief Represents a single checker piece on the board.
 */
class Piece
{
private:
    int player_id;        /**< ID of the player owning the piece (PLAYER1, PLAYER2, or NOPLAYER). */
    int y;                /**< Row number (top to bottom, 0-7). */
    int x;                /**< Column number (left to right, 0-7). */
    bool is_king;         /**< True if the piece is a king, false otherwise. */
    const char *color;    /**< ANSI color code for displaying the piece. */
    const char *selected; /**< ANSI background color code when the piece is selected. */

public:
    /**
     * @brief Constructs a Piece object.
     * Initializes player ID, coordinates, king status (default false), and colors based on player ID.
     * @param p_id Player ID (PLAYER1, PLAYER2, or NOPLAYER(==empty field)). Defaults to 0 (NOPLAYER).
     * @param y Initial row coordinate. Defaults to -1.
     * @param x Initial column coordinate. Defaults to -1.
     * @param king True if the piece is a king. Defaults to false.
     */
    Piece(int p_id = 0, int y = -1, int x = -1, bool king = false);

    /** @brief Gets the player ID of the piece. @return Player ID. */
    inline int get_id() const { return player_id; }
    /** @brief Gets the row coordinate of the piece. @return Row (0-7). */
    inline int get_y() const { return y; }
    /** @brief Gets the column coordinate of the piece. @return Column (0-7). */
    inline int get_x() const { return x; }
    /** @brief Checks if the piece is a king. @return True if king, false otherwise. */
    inline bool get_king() const { return is_king; }
    /** @brief Gets the ANSI color code of the piece. @return Color code string. */
    inline const char *get_color() const { return color; }
    /** @brief Gets the ANSI background color code when selected. @return Background color code string. */
    inline const char *get_selected() const { return selected; }

    /** @brief Sets the player ID. @param id New player ID. */
    void set_id(int id) { player_id = id; }
    /** @brief Sets the row coordinate. @param y_in New row. */
    void set_row(int y_in) { y = y_in; }
    /** @brief Sets the column coordinate. @param x_in New column. */
    void set_col(int x_in) { x = x_in; }
    /** @brief Sets the king status. @param choice True to make king, false otherwise. */
    void set_king(bool choice) { is_king = choice; }
    /** @brief Sets the display color. @param col New ANSI color code. */
    void set_color(const char *col) { color = col; }
    /** @brief Sets the selected background color. @param sel New ANSI background color code. */
    void set_selected(const char *sel) { selected = sel; }

    /**
     * @brief Sets the piece's appearance to indicate it is selected.
     * Changes the background color based on the player ID and sets the foreground color to bold white.
     * @param y Row coordinate of the selection context (e.g., enemy piece).
     * @param x Column coordinate of the selection context (e.g., enemy piece).
     */
    void select(int y, int x);
    /**
     * @brief Resets the piece's appearance to its default (deselected) state.
     * Restores the original player color and default background color.
     */
    void deselect();
    /**
     * @brief Checks if the piece represents an empty square.
     * @return True if the player ID is NOPLAYER (0), false otherwise.
     */
    bool is_empty() const;
    /**
     * @brief Prints detailed information about the piece to the console.
     * Includes coordinates, player ID, empty status, king status, and selection status.
     */
    void print_piece_info();
    /**
     * @brief Checks if given coordinates are within the valid board boundaries (0-7 inclusive).
     * @param y Row coordinate.
     * @param x Column coordinate.
     * @return True if both row and column are within bounds, false otherwise.
     */
    bool in_bounds(int y, int x) const;

    /**
     * @brief returns all of the piece's information in a string format for saving to a file.
     *
     * @return string
     */
    string info_to_file();
};

/**
 * @class Board
 * @brief Represents the 8x8 checkerboard.
 *
 * Contains the arrangement of pieces as a 2D array and counts of pieces for each player.
 */
class Board
{
private:
    array<array<Piece, 8>, 8> board; /**< 2D array representing the board state. */
    int num_player_pieces[2];        /**< Stores the number of pieces for [Player1, Player2]. */

public:
    /**
     * @brief Constructs a Board object from a 2D array of Pieces.
     * Initializes the board layout and counts the number of pieces for each player based on the input array.
     * @param arr_p An 8x8 std::array representing the initial board configuration.
     */
    Board(array<array<Piece, 8>, 8> arr_p);

    /**
     * @brief Default constructor for Board.
     * Initializes an empty board and sets piece counts to zero.
     */
    Board() : num_player_pieces{0, 0} { /* Initialize board if necessary */ }

    /**
     * @brief Gets the Piece at the specified coordinates.
     * @param y Row coordinate (0-7).
     * @param x Column coordinate (0-7).
     * @return A pointer to the Piece at the given location.
     */
    Piece* get_Piece(int y, int x);

    /**
     * @brief Clones the Piece at the specified coordinates.
     * @param y 
     * @param x 
     * @return Copy of the Piece at the given location.
     */
    Piece clone_Piece(int y, int x) const { return board[y][x]; }

    /**
     * @brief Prints the current state of the board to the console.
     * Displays the board with row and column numbers and uses ANSI colors for pieces and selection.
     */
    void print_Board();

    /**
     * @brief Places a piece at the specified coordinates on the board.
     * Updates the piece counts for the players based on the new piece.
     * @param y Row coordinate (0-7).
     * @param x Column coordinate (0-7).
     * @param piece The Piece object to place at the location.
     * @note This function is usually used for removing pieces from the board
     * TODO: rename function
     */
    void set_piece(int y, int x, const Piece &piece);

    /**
     * @brief Gets the number of pieces for a given player.
     * @param player_id Player ID (PLAYER1 or PLAYER2).
     * @return The current count of pieces for that player.
     */
    int get_num_players(int player_id) const
    {
        if (player_id == PLAYER1)
            return num_player_pieces[0];
        else if (player_id == PLAYER2)
            return num_player_pieces[1];
        return 0; // Should not happen for valid player IDs
    }

    /**
     * @brief Helper to get a modifiable piece reference.
     * Use with caution as it bypasses piece count management.
     * @param y Row coordinate (0-7).
     * @param x Column coordinate (0-7).
     * @return Reference to the Piece at the given location.
     */
    Piece &get_modifiable_piece(int y, int x) { return board[y][x]; }

    /**
     * @brief Get the board info object
     *
     * @return string
     */
    string get_board_info();

    /**
     * @brief 
     * 
     * @param mv 
     */
    void perform_move(Move mv)
    {
        // Perform the move on the board
        mv.perform_move(this, mv);
    }
    
};

/**
 * @class GameState
 * @brief Represents the complete state of the game at a point in time.
 *
 * Includes the board configuration, the current player whose turn it is,
 * and a list of all valid moves possible from this state for the current player.
 */
class GameState
{
private:
    Board board;        /**< The current board configuration. */
    int current_player; /**< ID of the player whose turn it is (PLAYER1 or PLAYER2). */
public:
    vector<Move> possible_moves; /**< List of all valid moves for the current player from this state. */
    /**
     * @brief Constructs a GameState object.
     * @param b The initial Board state.
     * @param curr_p The player whose turn it is (PLAYER1 or PLAYER2).
     * @param v_movs An optional initial list of possible moves (defaults to empty).
     */
    GameState(Board b, int curr_p, vector<Move> v_movs = {}) : board(b), current_player(curr_p), possible_moves(v_movs) {};

    /**
     * @brief Creates a deep copy (clone) of the current GameState.
     * This copies the board state (piece by piece), current player, and the list of possible moves.
     * @return A new GameState object with the same state as the original.
     */
    GameState clone() const;

    /**
     * @brief Determines if the current game state is terminal (a win, loss, or potentially draw).
     * A player loses if they have no pieces left or no possible moves.
     * @return PLAYER1 if Player 1 has won, PLAYER2 if Player 2 has won,
     * NOPLAYER (0) if the game is potentially a draw (no moves but pieces remain - needs refinement),
     * -1 if the game is not over yet.
     * @note The draw condition (returning 0) might need refinement based on specific Checkers rules (e.g., repetition, 50-move rule). Currently, it only checks for no possible moves when pieces still exist.
     */
    int TerminalState();

    /**
     * @brief Generates and stores all possible moves for the piece at the given coordinates in the `possible_moves` vector.
     *
     * Calculates valid diagonal moves (forward for regular pieces, both directions for kings).
     * It prioritizes jumps: if any jump moves are found for the piece, only jump moves are stored for that piece.
     * Otherwise, all valid non-jump moves are stored.
     *
     * @param from_y The starting row coordinate (0-7) of the piece.
     * @param from_x The starting column coordinate (0-7) of the piece.
     * @param player The ID of the player whose moves are being generated.
     * @note This function modifies the `possible_moves` member variable. If jumps are found, non-jump moves previously added for *other* pieces might still be present unless `list_all_possible_moves` clears the list first.
     */
    void list_possible_moves(int from_y, int from_x, int player);

    /**
     * @brief Generates and stores all possible moves for all pieces of the current player on the board.
     * Clears the existing `possible_moves` list first.
     * Then, iterates through all board squares. If a square contains a piece belonging to the `current_player`,
     * it calls `list_possible_moves(int from_x, int from_y)` for that piece.
     * @note This function ensures that `possible_moves` contains only valid moves for the *current* state and player, respecting jump priority across all pieces.
     */
    void list_all_possible_moves(int player);

    /**
     * @brief Prints all possible moves stored in the GameState to the console.
     */
    void print_all_moves();

    /**
     * @brief Gets the Board object associated with this GameState.
     * @return The Board object.
     */
    Board *get_board() { return &board; };

    void switch_player() { current_player = (current_player == PLAYER1) ? PLAYER2 : PLAYER1; } /**< Switches the current player. */

    int num_possible_moves() const { return possible_moves.size(); } /**< Returns the number of possible moves. */

    inline int get_current_player() const { return current_player; } /**< Returns the ID of the current player. */

    string get_state_info();
};

/**
 * @class MCTS_leaf
 * @brief Represents a node in the Monte Carlo Tree Search (MCTS) tree.
 *
 * Stores the game state reached by a specific move, statistics (wins/total games played through this node),
 * the UCB rating, and pointers for tree structure (parent/children).
 */
class MCTS_leaf
{
private:
    Move move;        /**< The move that led to this GameState from the parent node. */
    bool is_computer; /**< Flag indicating if the player to move in this state is the computer. */
    bool is_terminal; /**< Flag indicating if this node represents a terminal game state. */

public:
    GameState state;              /**< The GameState represented by this node. */
    MCTS_leaf *parent;            /**< Pointer to the parent node in the MCTS tree (nullptr for root). */
    vector<MCTS_leaf *> children; /**< Vector of pointers to child nodes. */
    int wins;                     /**< Number of simulated game wins passing through this node. */
    int total_games;              /**< Total number of simulated games passing through this node. */

    /**
     * @brief Constructs an MCTS_leaf node.
     * @param s The GameState represented by this node.
     * @param mv The Move that led to this state from the parent.
     * @param p Pointer to the parent node (defaults to nullptr for the root).
     * @param c Vector of child node pointers (defaults to an empty vector).
     * @param w Initial win count (simulations won passing through here). Defaults to 0.
     * @param tg Initial total game count (simulations played passing through here). Defaults to 0.
     * @param r Initial UCB rating of the leaf. Defaults to 0.0.
     * @param ic Flag indicating if the player to move in this state is the computer. Defaults to true.
     * @param it Flag indicating if this node represents a terminal game state. Defaults to false.
     */
    MCTS_leaf(GameState s, Move mv, MCTS_leaf *p = nullptr, vector<MCTS_leaf *> c = {}, int w = 0, int tg = 0, bool ic = true, bool it = false) : state(s), move(mv), wins(w), total_games(tg), is_computer(ic), is_terminal(it), parent(p), children(c) {};

    /** @brief Returns the number of direct children of this node. */
    int num_children() { return children.size(); };

    /**
     * @brief Calculates and updates the UCB1 rating for this node.
     * Uses the formula: rating = (wins / total_games) + C * sqrt(log(parent_total_games) / total_games).
     * If total_games is 0, sets rating to 0.
     */
    double cal_rating();

    /**
     * @brief writes the current state of the leaf node to a file.
     * @param out The output file stream to write to.
     */
    void save_leaf(ofstream &);

    void print_move() { move.print_move(); } /**< Prints the move associated with this node. */

    string get_move_info() { return move.get_move_info(); } /**< Returns the move information as a string. */
    // string get_state_info() { return state.get_state_info(); } /**< Returns the game state information as a string. */
};

#endif