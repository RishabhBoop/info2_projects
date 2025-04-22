/**
 * @file state.cpp
 * @brief Implements the classes defined in state.hpp for representing the checkers game state.
 */
#include "classes.hpp"

double C = sqrt(2);

Piece::Piece(int p_id, int y, int x, bool king) : player_id(p_id), y(y), x(x),
                                                  is_king(king),
                                                  color(p_id == 1 ? PLAYER1_COLOR : (p_id == 2 ? PLAYER2_COLOR : EMPTY_FIELD_COLOR)),
                                                  selected(NOT_SELECTED_FIELD_COLOR) {}

bool Piece::is_empty() const
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

void Piece::select(int y, int x)
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
    else if (id == 0)
    {
        set_selected(SELECTED_FIELD_COLOR);
        set_color(BOLDBLACK);
        return;
    }
    else if (y != -1 && x != -1)
    {
        set_selected(BGRED);
    }

    set_color(BOLDWHITE);

    return;
}

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

void Piece::print_piece_info()
{
    cout << get_color();
    printf("Piece [%d, %d]: \n", y + 1, x + 1);
    printf("\t y: %d\n", y);
    printf("\t x: %d\n", x);
    printf("\t player: %d\n", get_id());
    printf("\t is empty: %s\n", is_empty() ? "true" : "false");
    printf("\t is king: %s\n", is_king ? "true" : "false");
    printf("\t selected: %s\n", strcmp(get_selected(), NOT_SELECTED_FIELD_COLOR) ? "false" : "true");
    cout << RESET;
}

bool Piece::in_bounds(int y, int x) const
{
    return y >= 0 && y < 8 && x >= 0 && x < 8;
}

string Piece::info_to_file()
{
    string s = "";
    s += "p[" + to_string(player_id) + ", " + to_string(y) + ", " + to_string(x) + ", " + (is_king ? "1" : "0") + "]";
    return s;
}

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

void Board::set_piece(int y, int x, const Piece &piece)
{
    // Check if the existing piece belongs to a player
    if (board[y][x].get_id() == PLAYER1)
    {
        num_player_pieces[0]--;
    }
    else if (board[y][x].get_id() == PLAYER2)
    {
        num_player_pieces[1]--;
    }

    // Place the new piece
    board[y][x] = piece;

    // Check if the new piece belongs to a player
    if (piece.get_id() == PLAYER1)
    {
        num_player_pieces[0]++;
    }
    else if (piece.get_id() == PLAYER2)
    {
        num_player_pieces[1]++;
    }
}

void Board::print_Board()
{
    // #ifndef DEBUG
    // clear_screen();
    // #endif
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

string Board::get_board_info()
{
    string s = "";
    s += "[";
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            s += board[i][j].info_to_file();
            s += ",";
        }
    }
    s.pop_back();
    s += "]";
    return s;
}

void Move::print_move()
{
    printf("From [%d, %d] to [%d, %d]", src_y + 1, src_x + 1, dest_y + 1, dest_x + 1);
    if (jump)
    {
        printf(" while eliminating [%d, %d]", enemy_y + 1, enemy_x + 1);
    }
    printf("\n");
    return;
}

void GameState::print_all_moves()
{
    cout << "Possible moves:\n";
    for (int i = 0; i < possible_moves.size(); i++)
    {
        cout << "\t" << i + 1 << ". ";
        possible_moves[i].print_move();
    }
}

void GameState::list_all_possible_moves(int player)
{
    // clear the list of possible moves for the current player
    // this is needed because we keep on push-backing moves to the list
    possible_moves.clear();
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Piece* current_piece = board.get_Piece(i, j);
            if ((!current_piece->is_empty()) && (current_piece->get_id() == player))
            {
                list_possible_moves(i, j, player);
            }
        }
    }
    bool has_jump = false;
    for (Move m : possible_moves)
    {
        if (m.get_jump_type())
        {
            has_jump = true;
            break;
        }
    }
    if (has_jump)
    {
        for (int i = 0; i < possible_moves.size(); i++)
        {
            if (possible_moves[i].get_jump_type() == false)
            {
                possible_moves.erase(possible_moves.begin() + i);
                i--;
            }
        }
    }
}

void GameState::list_possible_moves(int from_y, int from_x, int player)
{
    bool jump = false;
    Piece current_piece = board.clone_Piece(from_y, from_x);
    int enemy = player == PLAYER1 ? PLAYER2 : PLAYER1;

    int directions[4][2] = {}; // "offset" for the directions; {dy,dx}
    int num_directions = 0;
    // move down
    if (player == PLAYER1 || current_piece.get_king())
    {
        // add down-left
        directions[num_directions][0] = 1;
        directions[num_directions][1] = -1;
        num_directions++;
        // add down-right
        directions[num_directions][0] = 1;
        directions[num_directions][1] = 1;
        num_directions++;
    }
    // move up
    if (player == PLAYER2 || current_piece.get_king())
    {
        // add up-left
        directions[num_directions][0] = -1;
        directions[num_directions][1] = -1;
        num_directions++;
        // add up-right
        directions[num_directions][0] = -1;
        directions[num_directions][1] = 1;
        num_directions++;
    }
    for (int i = 0; i < num_directions; i++)
    {
        int dy = directions[i][0];
        int dx = directions[i][1];
        int new_y = from_y + dy;
        int new_x = from_x + dx;
        // check if the new position is in bounds
        if (current_piece.in_bounds(new_y, new_x))
        {
            Piece new_piece = board.clone_Piece(new_y, new_x);
            // check if the new position is empty
            if (new_piece.is_empty())
            {
                // add the move to the list of possible moves
                Move mv(from_y, from_x, new_y, new_x, false, -1, -1);
                possible_moves.push_back(mv);
            }
            else if (!new_piece.is_empty() && new_piece.get_id() == enemy)
            {
                // check if the next position is in bounds
                int jump_y = new_y + dy;
                int jump_x = new_x + dx;
                if (current_piece.in_bounds(jump_y, jump_x))
                {
                    Piece jump_piece = board.clone_Piece(jump_y, jump_x);
                    // check if the next position is empty
                    if (jump_piece.is_empty())
                    {
                        // add the move to the list of possible moves
                        Move mv(from_y, from_x, jump_y, jump_x, true, new_y, new_x);
                        possible_moves.push_back(mv);
                        jump = true;
                    }
                }
            }
        }
    }

    return;
}

GameState GameState::clone() const
{
    // create new Board with the default values
    array<array<Piece, 8>, 8> new_board_data;
    for (int i = 0; i < 8; i++) // for each row
    {
        for (int j = 0; j < 8; j++) // for each column in that row
        {
            // copy the piece from the original board to the new board
            new_board_data[i][j] = board.clone_Piece(i, j);
        }
    }
    Board new_board(new_board_data);
    GameState new_game_state(new_board, current_player, {});
    return new_game_state;
}

int GameState::TerminalState()
{
    int player1_pieces = board.get_num_players(PLAYER1);
    int player2_pieces = board.get_num_players(PLAYER2);
    if (player2_pieces == 0)
    {
        // Player 1 wins
        return PLAYER1;
    }
    else if (player1_pieces == 0)
    {
        // Player 2 wins
        return PLAYER2;
    }
    // list_all_possible_moves(PLAYER2);
    if (possible_moves.empty())
    {
        // Player 2 has no moves, so Player 1 wins.
        return PLAYER1;
    }
    // list_all_possible_moves(PLAYER1);
    if (possible_moves.empty())
    {
        // Player 1 has no moves, so Player 2 wins.
        return PLAYER2;
    }
    // If neither player has lost, return -1 to indicate the game is still ongoing.
    return -1;
}

string GameState::get_state_info()
{
    string s = "";
    s += "[b";
    s += board.get_board_info();
    s += ",";
    s += to_string(current_player);
    s += "]";
    return s;
}

void Move::perform_move(Board *b, Move mv)
{
    // Get the piece which is being moved
    Piece moving_piece = b->clone_Piece(mv.src_y, mv.src_x);
    int player_id = moving_piece.get_id();

    // Create an empty piece to clear squares
    Piece empty_piece(NOPLAYER, -1, -1);

    // Clear the source square
    b->set_piece(mv.src_y, mv.src_x, empty_piece);

    // If it's a jump, clear the enemy square too
    if (mv.jump)
    {
        b->set_piece(mv.enemy_y, mv.enemy_x, empty_piece);
    }

    // Update the moving piece's coordinates
    moving_piece.set_row(mv.dest_y);
    moving_piece.set_col(mv.dest_x);

    // Check for king promotion
    if (!moving_piece.get_king())
    {
        if ((player_id == PLAYER1 && mv.dest_y == 7) || (player_id == PLAYER2 && mv.dest_y == 0))
        {
            moving_piece.set_king(true);
        }
    }

    // Place the piece at the destination
    b->set_piece(mv.dest_y, mv.dest_x, moving_piece);
}

string Move::get_move_info()
{
    string s = "";
    s += "[";
    s += to_string(src_y) + "," + to_string(src_x) + "," + to_string(dest_y) + "," + to_string(dest_x) + "," + to_string(jump) + "," + to_string(enemy_y) + "," + to_string(enemy_x);
    s += "]";
    return s;
}

double MCTS_leaf::cal_rating()
{
    double rating = 0;
    if (total_games != 0 && parent != nullptr)
    {
        // Parent's total games (Np) must also be positive for log
        if (parent->total_games > 0)
        {
            double np = static_cast<double>(parent->total_games);
            double nk = static_cast<double>(total_games);
            // Use this node's win rate (vk = wins / total_games)
            double vk = static_cast<double>(wins) / nk;
            // Exploration constant C (assuming it's defined elsewhere, e.g., as sqrt(2))
            // If C is not defined, you need to define it. Example: const double C = sqrt(2.0);
            const double C = sqrt(2.0); // Define C if not already defined globally/in scope
            double exploration_term = C * sqrt(log(np) / nk);
            rating = vk + exploration_term;
        }
        else
        {
            // Parent hasn't been visited? (unlikely)
            rating = INFINITY;
        }
    }
    else
    {
        rating = INFINITY;
    }
    return rating;
}

/* Format of output file:
r[g[GAMESTATE],wins,total_games,is_terminal,is_computer] -- only for root
c[m[MOVE],wins,total_games,is_terminal,is_computer] -- for all other nodes
r for root node (absolute parent of tree)
[GAMESTATE] = [b[BOARD],curr_player]
[BOARD] = [64*p[PIECE]]
[PIECE] = [player_id,y,x,is_king]
[MOVE] = [srcy,srcx,desty,destx,j,ey,ex]
inspiration: https://stackoverflow.com/questions/20005784/saving-a-binary-tree-to-a-file
*/
void MCTS_leaf::save_leaf(ofstream &out)
{
    if (parent == nullptr)
    {
        // if this is the root node, save r, gamestate
        out << "r" << "[g" << state.get_state_info() << ",";
    }
    else
    {
        // if this is not the root node, save c, move
        out << "c" << "[m" << move.get_move_info() << ",";
    }
    // save wins
    out << to_string(wins) << ",";
    // save total games
    out << to_string(total_games) << ",";
    // save is_terminal
    out << to_string(is_terminal) << ",";
    // save is_computer
    out << to_string(is_computer) << "]";
}

MCTS_leaf *load_leaf(string params, MCTS_leaf *parent)
{
    // the input string will be in the format:
    // char t = 'r' or 'c'
    // r/c[g/m[gamestate/move],wins,total_games,is_terminal,is_computer]
    char type = params[0];  // this is to see if the char is a 'r' or 'c'
    char type1 = params[2]; // this is to see if the char is a 'g' or 'm'

    params.erase(0, 3); // erase the first 3 characters (r/g[ or c/m[)
    if (type == 'c' && type1 == 'm' && parent != nullptr)
    {
        /* it will be in the format: "[...],wins,total_games,is_terminal,is_computer]" */
        params.erase(0, 1); // erase the first bracket
        // extract the values from the string
        int src_y = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        int src_x = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        int dest_y = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        int dest_x = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        bool jump = stoi(params.substr(0, params.find(','))) == 0 ? false : true;
        params.erase(0, params.find(',') + 1);
        int enemy_y = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        int enemy_x = stoi(params.substr(0, params.find(']')));
        params.erase(0, params.find(']') + 2); // erase the last bracket and comma
        // create a new move object
        Move new_move(src_y, src_x, dest_y, dest_x, jump, enemy_y, enemy_x);
        /* now the format will be "wins,...,is_computer"*/
        // get the wins
        int tmp_wins = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        int tmp_total_games = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        bool tmp_is_terminal = stoi(params.substr(0, params.find(','))) == 0 ? false : true;
        params.erase(0, params.find(',') + 1); // param string should now be empty
        bool tmp_is_computer = stoi(params.substr(0, params.find(']'))) == 0 ? false : true;
        params.erase(0, params.find(']') + 1); // erase the last bracket

        // clone gamestate from the parent
        GameState tmp_state = parent->state.clone();
        // switch the player
        tmp_state.switch_player();
        // perform the move
        Board *tmp_board = tmp_state.get_board();
        new_move.perform_move(tmp_board, new_move);

        // create a new MCTS_leaf object and attach to tree
        MCTS_leaf *new_leaf = new MCTS_leaf(tmp_state, new_move, parent, {}, tmp_wins, tmp_total_games, tmp_is_computer, tmp_is_terminal);
        // set the parent of the new leaf
        new_leaf->parent = parent;

        return new_leaf; // caller function should add this leaf to the parent
    }
    else if (type == 'r' && type1 == 'g' && parent == nullptr)
    {
        /* it will be in the format: "b[p[...],p[...]],curr_player],wins,total_games,rating,is_terminal]" */
        // get board (go until 'p')
        params.erase(0, 3);  // erase the first 2 characters (b[)        // extract the board string
        int tmp_counter = 1; // holds counter for bracket, so we can find mathcing bracket
        int i = 0;           // index for the string
        // parse until counter is 0
        while (tmp_counter != 0)
        {
            if (params[i] == '[')
            {
                tmp_counter++;
            }
            else if (params[i] == ']')
            {
                tmp_counter--;
            }
            i++;
        }
        int board_end = i; // end of the board (includes the last bracket; "...]]")

        string board_str = params.substr(0, board_end); // board_str will have the format p[...],...,p[...]]
        // parse the board and create a new board object
        array<array<Piece, 8>, 8> new_board_data;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                // erease the first 2 characters (p[)
                board_str.erase(0, 2);
                /* Each piece will now have the format: "...],p[...],...,p[...]]" */
                // get the player id
                int player_id = stoi(board_str.substr(0, board_str.find(',')));
                board_str.erase(0, board_str.find(',') + 1);
                // get the y coordinate
                int y = stoi(board_str.substr(0, board_str.find(',')));
                board_str.erase(0, board_str.find(',') + 1);
                // get the x coordinate
                int x = stoi(board_str.substr(0, board_str.find(',')));
                board_str.erase(0, board_str.find(',') + 1);
                // get the is_king
                bool is_king = stoi(board_str.substr(0, board_str.find(']'))) == 0 ? false : true;
                board_str.erase(0, board_str.find(']') + 2);
                // create a new piece object
                Piece new_piece(player_id, y, x, is_king);
                // set the piece in the new board
                new_board_data[i][j] = new_piece;
            }
        }
        // create a new board object
        Board new_board_obj(new_board_data);

        params.erase(0, board_end + 1);
        /* now the string params reads "curr_player],wins,total_games,rating,is_terminal,is_computer]" */
        int curr_player = stoi(params.substr(0, params.find(']')));
        // create a new game state object
        GameState new_game_state(new_board_obj, curr_player);

        params.erase(0, params.find(']') + 2); // erease the last bracket and comma

        /* now the string param reads "wins,...,is_terminal"*/
        int tmp_wins = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        int tmp_total_games = stoi(params.substr(0, params.find(',')));
        params.erase(0, params.find(',') + 1);
        bool tmp_is_terminal = stoi(params.substr(0, params.find(','))) == 0 ? false : true;
        params.erase(0, params.find(',') + 1); // param string should now be empty
        bool tmp_is_computer = stoi(params.substr(0, params.find(']'))) == 0 ? false : true;
        params.erase(0, params.find(']') + 1); // erase the last bracket

        // create a new MCTS_leaf object return it
        MCTS_leaf *new_leaf = new MCTS_leaf(new_game_state, Move(-1, -1, -1, -1, false, -1, -1), parent, {}, tmp_wins, tmp_total_games, tmp_is_computer, tmp_is_terminal);
        return new_leaf; // caller function should add this leaf to the parent
    }
    else
    {
        throw runtime_error("Invalid input format for MCTS_leaf");
        return nullptr;
    }
}