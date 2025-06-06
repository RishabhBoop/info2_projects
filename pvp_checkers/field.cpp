#include "field.hpp"

using namespace std;

int num_player_pieces[2] = {12,12};

/**
 * @brief Constructs a new field object.
 * 
 * Initializes a field square with its coordinates and creates an associated piece
 * with the specified player, color, and king status.
 *
 * @param y The row index (0-7, top to bottom).
 * @param x The column index (0-7, left to right).
 * @param is_black True if the underlying board square is black, false otherwise.
 * @param player The player occupying the field (NOPLAYER, PLAYER1, or PLAYER2).
 * @param color The ANSI color code string for displaying the piece.
 * @param is_king (Optional) True if the piece starts as a king. Defaults to false.
 * @param selected (Optional) The ANSI background color code string for when the field is selected. Defaults to NOT_SELECTED_FIELD_COLOR.
 */
field::field(const int y, const int x, const bool is_black, int player, const char *color, bool is_king, const char *selected)
{
    this->x = x; // coloumn number; left to right 0-7
    this->y = y; // row number; top to bottom 0-7
    this->myPiece.is_black = is_black;
    this->myPiece.is_king = is_king;
    if (player == NOPLAYER)
    {
        this->myPiece.player.player_id = 0;
        this->myPiece.player.color = color;
        this->myPiece.player.selected = selected;

    }
    else if (player == PLAYER1)
    {
        this->myPiece.player.player_id = 1;
        this->myPiece.player.color = color;
        this->myPiece.player.selected = selected;
    }
    else if (player == PLAYER2)
    {
        this->myPiece.player.player_id = 2;
        this->myPiece.player.color = color;
        this->myPiece.player.selected = selected;
    }
}

/**
 * @brief Prints detailed information about the field to the console.
 * 
 * Outputs the field's coordinates, player, king status, color codes, 
 * and whether it's considered empty or selected. Useful for debugging.
 * 
 * @param None
 * @return None
 */
void field::print_field()
{
    cout << myPiece.player.color;
    printf("Field [%d, %d]: \n", y + 1, x + 1);
    printf("\t y: %d\n", y);
    printf("\t x: %d\n", x);
    printf("\t player: %d\n", myPiece.player.player_id);
    printf("\t is empty: %s\n", is_empty(*this) ? "true" : "false");
    printf("\t is king: %s\n", myPiece.is_king ? "true" : "false");
    printf("\t is black: %s\n", myPiece.is_black ? "true" : "false");
    printf("\t selected: %s\n", myPiece.player.selected == NOT_SELECTED_FIELD_COLOR ? "false" : "true");
    cout << RESET;
    return;
}

/**
 * @brief Checks if given coordinates are within the board boundaries.
 *
 * @param row The row index to check.
 * @param col The column index to check.
 * @return true If both row and col are between 0 and 7 (inclusive).
 * @return false Otherwise.
 */
bool field::in_bounds(int row, int col)
{
    return row != -1 && row != 8 && col != -1 && col != 8;
}

/**
 * @brief Checks if a given field object is empty.
 *
 * A field is considered empty if its associated piece belongs to NOPLAYER (player ID 0).
 *
 * @param obj The field object to check.
 * @return true If the field is empty (obj.myPiece.player == NOPLAYER).
 * @return false If the field is occupied by PLAYER1 or PLAYER2.
 */
bool field::is_empty(field obj) { return obj.myPiece.player.player_id == 0 ? true : false; }

/**
 * @brief Calculates all possible moves (including jumps) for the piece on this field.
 *
 * Examines adjacent diagonal squares based on the piece's player and king status.
 * Identifies simple moves to empty squares and jumps over opponent pieces.
 * If a jump is possible, it filters out the normal moves (must jump).
 * Appends valid moves to the provided vector.
 *
 * @param board The current 8x8 game board state (array of field objects).
 * @param possible_moves A reference to a std::vector<int> where the calculated moves will be added.
 *                       Each move is represented by 5 consecutive integers:
 *                       - dest_y: Destination row index.
 *                       - dest_x: Destination column index.
 *                       - move_type: 0 for a normal move, 1 for a jump.
 *                       - enemy_y: Row index of the jumped enemy (-1 if not a jump).
 *                       - enemy_x: Column index of the jumped enemy (-1 if not a jump).
 * @return None (modifies the possible_moves vector directly).
 */
void field::list_possible_moves(std::array<std::array<field, 8>, 8>& board, vector<int> &possible_moves)
{   
    bool jump = false;
    // check for moving down
    if (myPiece.player.player_id == 1 || myPiece.player.player_id == 2 && myPiece.is_king)
    {
        // new y and x coordinates (move on the board diagonally to the left + bottom) 
        int y_fun = y + 1;
        int x_fun = x - 1;
        // if the next field is in bounds
        if (in_bounds(y_fun, x_fun))
        {
            field checking_field = board[y_fun][x_fun];
            int checking_player = checking_field.myPiece.player.player_id;
            // check if field is empty
            if (is_empty(checking_field))
            {
                // if the field is empty and not occupied by any player
                possible_moves.push_back(y_fun);    // top to bottom
                possible_moves.push_back(x_fun);    // left to right
                possible_moves.push_back(0);        // jump; 0 = normal; 1 = jump
                possible_moves.push_back(-1);       // -1 = enemy y doen't exist
                possible_moves.push_back(-1);       // -1 = enemy x doesn't exist
            }
            else if (!is_empty(checking_field) && checking_field.myPiece.player.player_id == 2)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun + 1;
                int x_fun_j = x_fun - 1;
                // check if the field behind the enemy is empty
                field checking_field = board[y_fun_j][x_fun_j];
                int checking_player = checking_field.myPiece.player.player_id;
                if (is_empty(checking_field) && checking_field.myPiece.player.player_id == 0)
                {
                    // if the field behind enemy is empty and not occupied by any player
                    possible_moves.push_back(y_fun_j);  // top to bottom
                    possible_moves.push_back(x_fun_j);  // left to right
                    possible_moves.push_back(1);        // jump; 0 = normal; 1 = jump
                    possible_moves.push_back(y_fun);    // top to bottom of enemy
                    possible_moves.push_back(x_fun);    // left to right of enemy
                    jump = true;
                }
            }
        }
        // new y and x coordinates (move on the board diagonally to the right + bottom)
        x_fun = x + 1;
        if (in_bounds(y_fun, x_fun))
        {
            field checking_field = board[y_fun][x_fun];
            int checking_player = checking_field.myPiece.player.player_id;
            // check if field is empty
            if (is_empty(checking_field) && checking_field.myPiece.player.player_id == 0)
            {
                // if the field is empty and not occupied by any player
                possible_moves.push_back(y_fun);    // top to bottom
                possible_moves.push_back(x_fun);    // left to right
                possible_moves.push_back(0);        // jump; 0 = normal; 1 = jump
                possible_moves.push_back(-1);       // -1 = enemy y doen't exist
                possible_moves.push_back(-1);       // -1 = enemy x doesn't exist
            }
            else if (!is_empty(checking_field) && checking_field.myPiece.player.player_id == 2)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun + 1;
                int x_fun_j = x_fun + 1;
                // check if the field behind the enemy is empty
                field checking_field = board[y_fun_j][x_fun_j];
                int checking_player = checking_field.myPiece.player.player_id;
                if (is_empty(checking_field) && checking_field.myPiece.player.player_id == 0)
                {
                    // if the field behind the enemy is empty and not occupied by any player
                    possible_moves.push_back(y_fun_j);  // top to bottom
                    possible_moves.push_back(x_fun_j);  // left to right
                    possible_moves.push_back(1);        // jump; 0 = normal; 1 = jump
                    possible_moves.push_back(y_fun);    // top to bottom of enemy
                    possible_moves.push_back(x_fun);    // left to right of enemy
                    jump = true;
                }
            }
        }
    }
    // check for moving up
    if (myPiece.player.player_id == 2 || myPiece.player.player_id == 1 && myPiece.is_king)
    {
        // new y and x coordinates (move on the board diagonally to the left + top)
        int y_fun = y - 1;
        int x_fun = x - 1;
        if (in_bounds(y_fun, x_fun))
        {
            field checking_field = board[y_fun][x_fun];
            int checking_player = checking_field.myPiece.player.player_id;
            // check if field is empty
            if (is_empty(checking_field))
            {
                // if the field is empty and not occupied by any player
                possible_moves.push_back(y_fun);    // top to bottom
                possible_moves.push_back(x_fun);    // left to right
                possible_moves.push_back(0);        // jump; 0 = normal; 1 = jump
                possible_moves.push_back(-1);       // -1 = enemy y doen't exist
                possible_moves.push_back(-1);       // -1 = enemy x doesn't exist
            }
            else if (!is_empty(checking_field) && checking_field.myPiece.player.player_id == 1)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun - 1;
                int x_fun_j = x_fun - 1;
                // check if the field behind the enemy is empty
                field checking_field = board[y_fun_j][x_fun_j];
                int checking_player = checking_field.myPiece.player.player_id;
                if (is_empty(checking_field) && checking_field.myPiece.player.player_id == 0)
                {
                    // if the field behind the enemy is empty and not occupied by any player
                    possible_moves.push_back(y_fun_j);  // top to bottom
                    possible_moves.push_back(x_fun_j);  // left to right
                    possible_moves.push_back(1);        // jump; 0 = normal; 1 = jump
                    possible_moves.push_back(y_fun);    // top to bottom of enemy
                    possible_moves.push_back(x_fun);    // left to right of enemy
                    jump = true;
                }
            }
        }
        // new y and x coordinates (move on the board diagonally to the right + top)
        x_fun = x + 1;
        if (in_bounds(y_fun, x_fun))
        {
            field checking_field = board[y_fun][x_fun];
            int checking_player = checking_field.myPiece.player.player_id;
            // check if field is empty
            if (is_empty(checking_field) && checking_field.myPiece.player.player_id == 0)
            {
                // if the field is empty and not occupied by any player
                possible_moves.push_back(y_fun);    // top to bottom
                possible_moves.push_back(x_fun);    // left to right
                possible_moves.push_back(0);        // jump; 0 = normal; 1 = jump
                possible_moves.push_back(-1);       // -1 = enemy y doen't exist
                possible_moves.push_back(-1);       // -1 = enemy x doesn't exist
            }
            else if (!is_empty(checking_field) && checking_field.myPiece.player.player_id == 1)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun - 1;
                int x_fun_j = x_fun + 1;
                // check if the field behind the enemy is empty
                field checking_field = board[y_fun_j][x_fun_j];
                int checking_player = checking_field.myPiece.player.player_id;
                if (is_empty(checking_field) && checking_field.myPiece.player.player_id == 0)
                {
                    // if the field behind the enemy is empty and not occupied by any player
                    possible_moves.push_back(y_fun_j);  // top to bottom
                    possible_moves.push_back(x_fun_j);  // left to right
                    possible_moves.push_back(1);        // jump; 0 = normal; 1 = jump
                    possible_moves.push_back(y_fun);    // top to bottom of enemy
                    possible_moves.push_back(x_fun);    // left to right of enemy
                    jump = true;
                }
            }
        }
    }
    // if the piece can jump, remove the normal moves
    if (jump)
    {
        for (int i = 0; i < possible_moves.size(); i += 5)
        {
            if (possible_moves[i + 2] == 0)
            {
                possible_moves.erase(possible_moves.begin() + i, possible_moves.begin() + i + 5);
                i -= 5; // adjust the index after erasing, as all of the elements are shifted after erasing
            }
        }
    }
    
    return;
}

/**
 * @brief Marks this field as selected for display purposes.
 * 
 * Updates the background color (`selected`) and foreground color (`color`) 
 * of the field's piece to visually indicate selection in the console output.
 * The background color depends on the player occupying the field.
 * 
 * @param None
 * @return None
 */
void field::select()
{
    if (myPiece.player.player_id == 1)
    {
        myPiece.player.selected = PLAYER1_COLOR_BG;
    }
    else if (myPiece.player.player_id == 2)
    {
        myPiece.player.selected = PLAYER2_COLOR_BG;
    }
    else
    {
        myPiece.player.selected = SELECTED_FIELD_COLOR;
    }
    myPiece.player.color = BOLDWHITE;
    return;
}

/**
 * @brief Resets the selection state of this field.
 * 
 * Restores the original background and foreground colors of the field's piece, 
 * removing the visual selection indicator.
 * 
 * @param None
 * @return None
 */
void field::unselect()
{
    myPiece.player.selected = NOT_SELECTED_FIELD_COLOR;
    if (myPiece.player.player_id == 1)
    {
        myPiece.player.color = PLAYER1_COLOR;
    }
    else if (myPiece.player.player_id == 2)
    {
        myPiece.player.color = PLAYER2_COLOR;
    }
    else
    {
        myPiece.player.color = EMPTY_FIELD_COLOR;
    }
    return;
}

/**
 * @brief Executes a piece move from this field to a destination field.
 *
 * Handles the logic for moving the piece, removing jumped opponents, 
 * and promoting pieces to kings if they reach the opposite end of the board.
 *
 * @param dest_y The destination row index.
 * @param dest_x The destination column index.
 * @param board The game board array, which will be modified by the move.
 * @param jump True if the move is a jump, false otherwise.
 * @param enemy_y The row index of the enemy piece being jumped (-1 if not a jump).
 * @param enemy_x The column index of the enemy piece being jumped (-1 if not a jump).
 * @return None (modifies the board state directly).
 */
void field::move(int dest_y, int dest_x, std::array<std::array<field, 8>, 8>& board, bool jump, int enemy_y, int enemy_x)
{
    // if jump, then remove the enemy piece
    if (jump)
    {
        // set enemy field to normal field
        board[enemy_y][enemy_x].myPiece.player.color = EMPTY_FIELD_COLOR;
        board[enemy_y][enemy_x].myPiece.player.selected = NOT_SELECTED_FIELD_COLOR;
        board[enemy_y][enemy_x].myPiece.player.player_id = 0;
        board[enemy_y][enemy_x].myPiece.is_black = false;
        board[enemy_y][enemy_x].myPiece.is_king = false;
        // decrement the number of pieces for the player
        if (myPiece.player.player_id == 1)
        {
            num_player_pieces[0]--;
        }
        else if (myPiece.player.player_id == 2)
        {
            num_player_pieces[1]--;
        }
    }

    // store the current piece in a temporary variable
    Piece myTmpPiece = board[y][x].myPiece;
    // set the current field to empty (which should be the desination field)
    board[y][x].myPiece = board[dest_y][dest_x].myPiece;
    // set destination field
    board[dest_y][dest_x].myPiece = myTmpPiece;
    
    // promote to king if the piece reaches the last row
    if (board[dest_y][dest_x].myPiece.player.player_id == 1 && dest_y == 7)
    {
        board[dest_y][dest_x].myPiece.is_king = true;
    }
    else if (board[dest_y][dest_x].myPiece.player.player_id == 2 && dest_y == 0)
    {
        board[dest_y][dest_x].myPiece.is_king = true;
    }

    return;
}

/**
 * @brief Clears the console screen.
 * 
 * Uses system-specific commands ("clear" for Linux/macOS, "cls" for Windows) 
 * based on the OS_LINUX preprocessor definition.
 * 
 * @param None
 * @return None
 */
void clear_screen()
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
 * @brief Prints the entire game board to the console.
 * 
 * Clears the screen first, then renders the 8x8 board using the colors 
 * defined in each field's piece. Includes row and column labels.
 *
 * @param board The 8x8 game board array (containing field objects) to print.
 * @return None
 */
void print_board(std::array<std::array<field, 8>, 8>& board)
{
    clear_screen();
    cout << "    1   2   3   4   5   6   7   8\n";
    cout << "  ---------------------------------\n";
    for (int i = 0; i < 8; i++)
    {
        cout << i + 1 << " | ";
        for (int j = 0; j < 8; j++)
        {
            cout  << board[i][j].myPiece.player.selected << board[i][j].myPiece.player.color << board[i][j].myPiece.player.player_id  << RESET << " | ";
        }
        cout << endl;
        cout << "  ---------------------------------\n";
    }

    return;
}

/**
 * @brief Prints a list of possible moves for a given field to the console.
 *
 * Formats and displays the contents of a `possible_moves` vector in a 
 * human-readable way, indicating destination, move type, and jumped enemy.
 *
 * @param x The column index of the field whose moves are being printed (for context).
 * @param y The row index of the field whose moves are being printed (for context).
 * @param possible_moves A reference to the vector containing the possible moves (5 integers per move).
 * @return None
 */
void print_possible_moves(int x, int y, vector<int> &possible_moves)
{
    int v_move = 0;
    printf("Possible moves for field [%d, %d]: \n", y + 1, x + 1);
    while (v_move < possible_moves.size())
    {
        cout << "\t[y, x]: [" << possible_moves[v_move] + 1 << ", " << possible_moves[v_move + 1] + 1 << "] ";
        printf("\n\t\tjump: %s\n", possible_moves[v_move + 2] == 0 ? "normal field" : "jumped enemy");
        if (possible_moves[v_move + 3] != -1 && possible_moves[v_move + 4] != -1)
        {
            printf("\t\tenemy field: [%d, %d]\n", possible_moves[v_move + 3] + 1, possible_moves[v_move + 4] + 1);
        }
        else
        {
            printf("\t\tenemy field: [none]\n");
        }
        v_move = v_move + 5;
    }

    return;
}

/**
 * @brief Checks if a player has lost the game.
 *
 * A player is considered to have lost if they have no pieces left on the board.
 *
 * @param player The player ID to check (1 or 2).
 * @return true If the player has lost (no pieces left).
 * @return false If the player still has pieces on the board.
 */
bool has_lost(int player)
{
    if (player == 1 && num_player_pieces[1] == 0)
    {
        return true;
    }
    else if (player == 2 && num_player_pieces[0] == 0)
    {
        return true;
    }
    return false;
}