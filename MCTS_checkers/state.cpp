/**
 * @file state.cpp
 * @brief Implements the classes defined in state.hpp for representing the checkers game state.
 */
#include "state.hpp"

/**
 * @brief Constructs a Piece object.
 * Initializes player ID, coordinates, king status, and colors based on player ID.
 * @param p_id Player ID (PLAYER1, PLAYER2, or NOPLAYER).
 * @param x Initial column coordinate.
 * @param y Initial row coordinate.
 * @param king True if the piece is a king, false otherwise.
 */
Piece::Piece(int p_id, int x, int y, bool king) : player_id(p_id), x(x), y(y),
                                                  is_king(king),
                                                  color(p_id == 1 ? PLAYER1_COLOR : (p_id == 2 ? PLAYER2_COLOR : EMPTY_FIELD_COLOR)),
                                                  selected(NOT_SELECTED_FIELD_COLOR) {}

/**
 * @brief Checks if the piece represents an empty square.
 * @return True if the player ID is NOPLAYER (0), false otherwise.
 */
bool Piece::is_empty()
{
    if (player_id == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Sets the piece's appearance to indicate it is selected.
 * Changes the background color based on the player ID and sets the foreground color to bold white.
 */
void Piece::select()
{
    int id = get_id();
    // set color based on player id
    if (id == 1)
    {
        set_selected(PLAYER1_COLOR_BG);
    }
    else if (id == 2)
    {
        set_selected(PLAYER2_COLOR_BG);
    }
    else
    {
        set_selected(SELECTED_FIELD_COLOR);
    }

    set_color(BOLDWHITE);

    return;
}

/**
 * @brief Resets the piece's appearance to its default (deselected) state.
 * Restores the original player color and default background color.
 */
void Piece::deselect()
{
    int id = get_id();
    if (id == 1)
    {
        set_color(PLAYER1_COLOR);
    }
    else if (id == 2)
    {
        set_color(PLAYER2_COLOR);
    }
    else
    {
        set_color(EMPTY_FIELD_COLOR);
    }
    set_selected(NOT_SELECTED_FIELD_COLOR);

    return;
}

/**
 * @brief Prints detailed information about the piece to the console.
 * Includes coordinates, player ID, empty status, king status, and selection status.
 */
void Piece::print_piece_info()
{
    cout << get_color();
    printf("Piece [%d, %d]: \n", y + 1, x + 1);
    printf("\t y: %d\n", y);
    printf("\t x: %d\n", x);
    printf("\t player: %d\n", get_id());
    printf("\t is empty: %s\n", is_empty() ? "true" : "false");
    printf("\t is king: %s\n", is_king ? "true" : "false");
    printf("\t selected: %s\n", get_selected() == NOT_SELECTED_FIELD_COLOR ? "false" : "true");
    cout << RESET;
}

/**
 * @brief Constructs a Board object from a 2D array of Pieces.
 * Initializes the board layout and counts the number of pieces for each player.
 * @param arr_p An 8x8 std::array representing the initial board configuration.
 */
Board::Board(array<array<Piece, 8>, 8> arr_p) : board(arr_p)
{
    num_player_pieces[0] = 0; // Initialize Player 1 count
    num_player_pieces[1] = 0; // Initialize Player 2 count
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (board[i][j].get_id() == PLAYER1)
            {
                num_player_pieces[0]++;
            }
            else if (board[i][j].get_id() == PLAYER2)
            {
                num_player_pieces[1]++;
            }
        }
    }
}

/**
 * @brief Sets (places or replaces) a piece at the specified coordinates on the board.
 * @param y Row coordinate (0-7).
 * @param x Column coordinate (0-7).
 * @param piece The Piece object to place at the location.
 */
void Board::set_piece(int y, int x, Piece piece)
{
    board[y][x] = piece;
}

/**
 * @brief Checks if given coordinates are within the valid board boundaries.
 * @param row Row coordinate.
 * @param col Column coordinate.
 * @return True if both row and column are between 0 and 7 (inclusive), false otherwise.
 */
bool Piece::in_bounds(int row, int col)
{
    return row != -1 && row != 8 && col != -1 && col != 8;
}

/**
 * @brief Clears the console screen.
 * Uses `clear` on Linux/macOS and `cls` on Windows.
 */
void Board::clear_screen()
{
    if (OS_LINUX)
    {
        system("clear");
    }
    else if (OS_LINUX == 0)
    {
        system("cls");
    }
    return;
}

/**
 * @brief Prints the current state of the board to the console.
 * Displays the board with row and column numbers and uses ANSI colors for pieces and selection.
 */
void Board::print_Board()
{

    clear_screen();
    cout << "    1   2   3   4   5   6   7   8\n";
    cout << "  ---------------------------------\n";
    for (int i = 0; i < 8; i++) // for each row
    {
        cout << i + 1 << " | ";
        for (int j = 0; j < 8; j++) // for each column in that row
        {
            cout << board[i][j].get_selected() << board[i][j].get_color() << board[i][j].get_id() << RESET << " | ";
        }
        cout << endl;
        cout << "  ---------------------------------\n";
    }
}

/**
 * @brief Prints all possible moves stored in the GameState to the console.
 */
void GameState::print_all_moves()
{
    for (int i = 0; i < possible_moves.size(); i++)
    {
        possible_moves[i].print_move();
    }
}

/**
 * @brief Generates and stores all possible moves for the piece at the given coordinates.
 * 
 * This function calculates valid diagonal moves (forward for regular pieces, both directions for kings).
 * It prioritizes jumps: if any jump moves are found for the piece, only jump moves are stored in the `possible_moves` vector.
 * Otherwise, all valid non-jump moves are stored.
 * 
 * @param from_x The starting column coordinate (0-7) of the piece.
 * @param from_y The starting row coordinate (0-7) of the piece.
 */
void GameState::list_possible_moves(int from_x, int from_y)
{
    bool jump = false;
    // check for moving down
    if (board.get_Piece(from_y, from_x).get_id() == 1 || board.get_Piece(from_y, from_x).get_id() == 2 && board.get_Piece(from_y, from_x).get_king())
    {
        // new y and x coordinates (move on the board diagonally to the left + bottom)
        int y_fun = from_y + 1;
        int x_fun = from_x - 1;
        // if the next field is in bounds
        if (board.get_Piece(y_fun, x_fun).in_bounds(y_fun, x_fun))
        {
            // check if next field is empty
            if (board.get_Piece(y_fun, x_fun).is_empty())
            {
                // if the field is empty and not occupied by any player
                // create a move object
                Move new_move(from_x, from_y, x_fun, y_fun, 0, -1, -1);
                // add the move to the list of possible moves
                possible_moves.push_back(new_move);
            }
            else if (!board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 2)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun + 1;
                int x_fun_j = x_fun - 1;
                // check if the field behind the enemy is empty
                if (board.get_Piece(y_fun, x_fun).is_empty())
                {
                    Move new_move(from_x, from_y, x_fun, y_fun, 1, y_fun_j, x_fun_j);
                    // if the field behind enemy is empty and not occupied by any player
                    possible_moves.push_back(new_move);
                    jump = true;
                }
            }
        }
        // new y and x coordinates (move on the board diagonally to the right + bottom)
        x_fun = from_x + 1;
        if (board.get_Piece(y_fun, x_fun).in_bounds(y_fun, x_fun))
        {
            // check if field is empty
            if (board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 0)
            {
                // if the field is empty and not occupied by any player
                Move new_move(from_x, from_y, x_fun, y_fun, 0, -1, -1);
                // add the move to the list of possible moves
                possible_moves.push_back(new_move);
            }
            else if (!board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 2)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun + 1;
                int x_fun_j = x_fun + 1;
                // check if the field behind the enemy is empty
                if (board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 0)
                {
                    // if the field behind enemy is empty and not occupied by any player
                    Move new_move(from_x, from_y, x_fun, y_fun, 1, y_fun_j, x_fun_j);
                    possible_moves.push_back(new_move);
                    jump = true;
                }
            }
        }
    }
    // check for moving up
    if (board.get_Piece(from_y, from_x).get_id() == 2 || board.get_Piece(from_y, from_x).get_id() == 1 && board.get_Piece(from_y, from_x).get_king())
    {
        // new y and x coordinates (move on the board diagonally to the left + top)
        int y_fun = from_y - 1;
        int x_fun = from_x - 1;
        if (board.get_Piece(y_fun, x_fun).in_bounds(y_fun, x_fun))
        {
            if (board.get_Piece(y_fun, x_fun).is_empty())
            {
                // if the field is empty and not occupied by any player
                // create move object
                // create a move object
                Move new_move(from_x, from_y, x_fun, y_fun, 0, -1, -1);
                // add the move to the list of possible moves
                possible_moves.push_back(new_move);
            }
            else if (!board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 1)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun - 1;
                int x_fun_j = x_fun - 1;
                if (board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 0)
                {
                    // if the field behind the enemy is empty and not occupied by any player
                    Move new_move(from_x, from_y, x_fun, y_fun, 1, y_fun_j, x_fun_j);
                    possible_moves.push_back(new_move);
                    jump = true;
                }
            }
        }
        // new y and x coordinates (move on the board diagonally to the right + top)
        x_fun = from_x + 1;
        if (board.get_Piece(y_fun, x_fun).in_bounds(y_fun, x_fun))
        {
            // check if field is empty
            if (board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 0)
            {
                // if the field is empty and not occupied by any player
                // create a move object
                Move new_move(from_x, from_y, x_fun, y_fun, 0, -1, -1);
                // add the move to the list of possible moves
                possible_moves.push_back(new_move);
            }
            else if (!board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 1)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun - 1;
                int x_fun_j = x_fun + 1;
                if (board.get_Piece(y_fun, x_fun).is_empty() && board.get_Piece(y_fun, x_fun).get_id() == 0)
                {
                    // if the field behind the enemy is empty and not occupied by any player
                    Move new_move(from_x, from_y, x_fun, y_fun, 1, y_fun_j, x_fun_j);
                    possible_moves.push_back(new_move);
                    jump = true;
                }
            }
        }
    }
    // if piece can jump, remove the normal moves
    if (jump)
    {
        for (int i = 0; i < possible_moves.size(); i++)
        {
            if (possible_moves[i].get_jump_type())
            {
                possible_moves.erase(possible_moves.begin() + i);
                i--; // adjust the index after erasing, as all of the elements are shifted after erasing
            }
        }
    }
    return;
}

/**
 * @brief Creates a deep copy (clone) of the current GameState.
 * This involves copying the board state, current player, and the list of possible moves.
 * @return A new GameState object with the same state as the original.
 */
GameState GameState::clone() const
{
    // create new Board with the default values
    array<array<Piece, 8>, 8> new_board;
    for (int i = 0; i < 8; i++) // for each row
    {
        for (int j = 0; j < 8; j++) // for each column in that row
        {
            // copy the piece from the original board to the new board
            new_board[i][j] = board.get_Piece(i, j);
        }
    }
    GameState new_game_state(new_board, current_player, possible_moves);
    return new_game_state;
}

/**
 * @brief Determines if the current game state is terminal (a win or loss).
 * A player loses if they have no pieces left or no possible moves.
 * @return PLAYER1 if Player 1 has won, PLAYER2 if Player 2 has won, NOPLAYER (0) if the game is not over.
 */
int GameState::TerminalState()
{
    // player has lost if they have no pieces left on the board or no possible moves
    if (current_player + 1 == 2 && (possible_moves.size() == 0 || board.get_num_players(current_player + 1) == 0)) // is currentPlayer 1
    {
        return 1; // player 1 won
    }
    else if (current_player - 1 == 1 && (possible_moves.size() == 0 || board.get_num_players(current_player - 1) == 0))
    {
        return 2; // player 2 won
    }
    return 0; // no one has won
}

/**
 * @brief Prints the details of a single move to the console.
 * Shows source and destination coordinates, and jumped piece coordinates if applicable.
 */
void Move::print_move()
{
    printf("From [%d, %d] to [%d, %d]", src_x, src_y, dest_x, dest_y);
    if (jump)
    {
        printf(" while eliminating [%d, %d]", enemy_x, enemy_y);
    }
    printf("\n");
    return;
}

/**
 * @brief Performs the specified move on the given board state.
 * 
 * Updates the piece positions on the board. If the move is a jump,
 * it removes the jumped enemy piece and updates the piece count for the opponent.
 * Note: This function currently modifies a copy of the board (`Board b`) passed by value.
 * To modify the original board within a GameState, it should likely take a `Board&` reference.
 * 
 * @param b The Board object to perform the move on (passed by value).
 * @param mv The Move object describing the move to perform.
 */
void Move::perform_move(Board b, Move mv)
{
    // get source and enemy player id
    int tmp_src_player = b.get_Piece(mv.src_y, mv.src_x).get_id();
    int tmp_enemy_player = tmp_src_player == 1 ? 2 : 1;
    // change the src piece position
    b.get_Piece(mv.src_y, mv.src_x).set_x(dest_x);
    b.get_Piece(mv.src_y, mv.src_x).set_x(dest_y);
    // set the destination field to the src piece
    b.get_Piece(mv.dest_y, mv.dest_x).set_x(src_x);
    b.get_Piece(mv.dest_y, mv.dest_x).set_x(src_y);
    // if jump, then remove the enemy piece
    if (jump)
    {
        // set enemy field to normal field
        b.get_Piece(mv.enemy_y, mv.enemy_x).set_color(EMPTY_FIELD_COLOR);
        b.get_Piece(mv.enemy_y, mv.enemy_x).set_selected(NOT_SELECTED_FIELD_COLOR);
        b.get_Piece(mv.enemy_y, mv.enemy_x).set_id(0);
        b.get_Piece(mv.enemy_y, mv.enemy_x).set_king(false);
        // decrement the number of pieces for the player
        int new_num_pieces = b.get_num_players(tmp_enemy_player);
        new_num_pieces--;
        b.set_num_players(tmp_enemy_player, new_num_pieces);
    }
    return;
}