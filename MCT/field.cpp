#include "field.hpp"

using namespace std;

field::field(const int y, const int x, const bool is_black, const int player, const char *color, bool is_king, const char *selected)
{
    this->x = x; // coloumn number; left to right 0-7
    this->y = y; // row number; top to bottom 0-7
    this->is_black = is_black;
    this->player = player;
    this->is_king = is_king;
    this->color = color;
    this->selected = selected;
}

void field::print_field()
{
    clear_screen();
    cout << color;
    printf("Field [%d, %d]: \n", y + 1, x + 1);
    printf("\t y: %d\n", y);
    printf("\t x: %d\n", x);
    printf("\t player: %d\n", player);
    printf("\t is empty: %s\n", is_empty(*this) ? "true" : "false");
    printf("\t is king: %s\n", is_king ? "true" : "false");
    printf("\t is black: %s\n", is_black ? "true" : "false");
    printf("\t selected: %s\n", selected ? "true" : "false");
    cout << RESET;
    return;
}

void field::tmp_print_any_field(field obj)
{
    cout << color;
    printf("Field [%d, %d]: \n", obj.y + 1, obj.x + 1);
    printf("\t y: %d\n", obj.y);
    printf("\t x: %d\n", obj.x);
    printf("\t player: %d\n", obj.player);
    printf("\t is empty: %s\n", is_empty(obj) ? "true" : "false");
    printf("\t is king: %s\n", obj.is_king ? "true" : "false");
    printf("\t is black: %s\n", obj.is_black ? "true" : "false");
    printf("\t selected: %s\n", obj.selected ? "true" : "false");
    cout << RESET;
    return;
}

bool field::in_bounds(int row, int col)
{
    return row != -1 && row != 8 && col != -1 && col != 8;
}

bool field::is_empty(field obj) { return obj.player == 0 ? true : false; }

void field::list_possible_moves(field board[8][8], vector<int> &possible_moves)
{
    if (player == 1 || player == 2 && is_king)
    {
        int y_fun = y + 1;

        int x_fun = x - 1;
        if (in_bounds(y_fun, x_fun))
        {
            field checking_field = board[y_fun][x_fun];
            int checking_player = checking_field.player;
            // check if field is empty
            if (is_empty(checking_field) && checking_field.player == 0)
            {
                // if the field is empty and not occupied by any player
                possible_moves.push_back(y_fun); // top to bottom
                possible_moves.push_back(x_fun); // left to right
                possible_moves.push_back(0);     // jump; 0 = normal; 1 = jump
            }
            else if (is_empty(checking_field) == false && checking_field.player == 2)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun + 1;
                int x_fun_j = x_fun - 1;
                // check if the field behind the enemy is empty
                field checking_field = board[y_fun_j][x_fun_j];
                int checking_player = checking_field.player;
                if (is_empty(checking_field) && checking_field.player == 0)
                {
                    // if the field is empty and not occupied by any player
                    possible_moves.push_back(y_fun_j); // top to bottom
                    possible_moves.push_back(x_fun_j); // left to right
                    possible_moves.push_back(1);       // jump; 0 = normal; 1 = jump
                }
            }
        }

        x_fun = x + 1;
        if (in_bounds(y_fun, x_fun))
        {
            field checking_field = board[y_fun][x_fun];
            int checking_player = checking_field.player;
            // check if field is empty
            if (is_empty(checking_field) && checking_field.player == 0)
            {
                // if the field is empty and not occupied by any player
                possible_moves.push_back(y_fun); // top to bottom
                possible_moves.push_back(x_fun); // left to right
                possible_moves.push_back(0);     // jump; 0 = normal; 1 = jump
            }
            else if (is_empty(checking_field) == false && checking_field.player == 2)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun + 1;
                int x_fun_j = x_fun + 1;
                // check if the field behind the enemy is empty
                field checking_field = board[y_fun_j][x_fun_j];
                int checking_player = checking_field.player;
                if (is_empty(checking_field) && checking_field.player == 0)
                {
                    // if the field is empty and not occupied by any player
                    possible_moves.push_back(y_fun_j); // top to bottom
                    possible_moves.push_back(x_fun_j); // left to right
                    possible_moves.push_back(1);       // jump; 0 = normal; 1 = jump
                }
            }
        }
    }
    if (player == 2 || player == 1 && is_king)
    {
        int y_fun = y - 1;

        int x_fun = x - 1;
        if (in_bounds(y_fun, x_fun))
        {
            field checking_field = board[y_fun][x_fun];
            int checking_player = checking_field.player;
            // check if field is empty
            if (is_empty(checking_field) && checking_field.player == 0)
            {
                // if the field is empty and not occupied by any player
                possible_moves.push_back(y_fun); // top to bottom
                possible_moves.push_back(x_fun); // left to right
                possible_moves.push_back(0);     // jump; 0 = normal; 1 = jump
            }
            else if (is_empty(checking_field) == false && checking_field.player == 1)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun - 1;
                int x_fun_j = x_fun - 1;
                // check if the field behind the enemy is empty
                field checking_field = board[y_fun_j][x_fun_j];
                int checking_player = checking_field.player;
                if (is_empty(checking_field) && checking_field.player == 0)
                {
                    // if the field is empty and not occupied by any player
                    possible_moves.push_back(y_fun_j); // top to bottom
                    possible_moves.push_back(x_fun_j); // left to right
                    possible_moves.push_back(1);       // jump; 0 = normal; 1 = jump
                }
            }
        }

        x_fun = x + 1;
        if (in_bounds(y_fun, x_fun))
        {
            field checking_field = board[y_fun][x_fun];
            int checking_player = checking_field.player;
            // check if field is empty
            if (is_empty(checking_field) && checking_field.player == 0)
            {
                // if the field is empty and not occupied by any player
                possible_moves.push_back(y_fun); // top to bottom
                possible_moves.push_back(x_fun); // left to right
                possible_moves.push_back(0);     // jump; 0 = normal; 1 = jump
            }
            else if (is_empty(checking_field) == false && checking_field.player == 1)
            {
                // if the field is occupied by the enemy
                int y_fun_j = y_fun - 1;
                int x_fun_j = x_fun + 1;
                // check if the field behind the enemy is empty
                field checking_field = board[y_fun_j][x_fun_j];
                int checking_player = checking_field.player;
                if (is_empty(checking_field) && checking_field.player == 0)
                {
                    // if the field is empty and not occupied by any player
                    possible_moves.push_back(y_fun_j); // top to bottom
                    possible_moves.push_back(x_fun_j); // left to right
                    possible_moves.push_back(1);       // jump; 0 = normal; 1 = jump
                }
            }
        }
    }

    return;
}

void field::select()
{
    if (player == 1)
    {
        selected = PLAYER1_COLOR_BG;
    }
    else if (player == 2)
    {
        selected = PLAYER2_COLOR_BG;
    }
    else
    {
        selected = SELECTED_FIELD_COLOR;
    }
    color = BOLDWHITE;
    return;
}

void field::unselect()
{
    selected = NOT_SELECTED_FIELD_COLOR;
    if (player == 1)
    {
        color = PLAYER1_COLOR;
    }
    else if (player == 2)
    {
        color = PLAYER2_COLOR;
    }
    else
    {
        color = EMPTY_FIELD_COLOR;
    }
    return;
}

void field::move(int dest_y, int dest_x, field board[8][8], bool jump, int enemy_y, int enemy_x)
{
    // if jump, then remove the enemy piece
    if (jump)
    {
        board[enemy_y][enemy_x].player = 0;
        board[enemy_y][enemy_x].color = EMPTY_FIELD_COLOR;
    }

    // set destination field
    board[dest_y][dest_x].player = player;
    board[dest_y][dest_x].color = color;

    // set current field
    player = 0;
    color = EMPTY_FIELD_COLOR;
    return;
}

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

void print_board(field board[8][8])
{
    cout << "    1   2   3   4   5   6   7   8\n";
    cout << "  ---------------------------------\n";
    for (int i = 0; i < 8; i++)
    {
        cout << i + 1 << " | ";
        for (int j = 0; j < 8; j++)
        {
            cout  << board[i][j].selected << board[i][j].color << board[i][j].player << RESET << " | ";
        }
        cout << endl;
        cout << "  ---------------------------------\n";
    }

    return;
}

#if NCURSES
void print_board_ncurses(field board[8][8])
{
    printw("    1   2   3   4   5   6   7   8\n");
    printw("  ---------------------------------\n");
    for (int i = 0; i < 8; i++)
    {
        cout << i + 1 << " | ";
        for (int j = 0; j < 8; j++)
        {
            printw(board[i][j].color, "%d", board[i][j].player, RESET, " | ");
        }
        printw("\n");
        printw("  ---------------------------------\n");
    }

    return;
}
#endif

void print_possible_moves(int x, int y, vector<int> &possible_moves)
{
    int v_move = 0;
    printf("Possible moves for field [%d, %d]: \n", y + 1, x + 1);
    while (v_move < possible_moves.size())
    {
        cout << "\t[y, x]: [" << possible_moves[v_move] + 1 << ", " << possible_moves[v_move + 1] + 1 << "] ";
        printf("jump: %s\n", possible_moves[v_move + 2] == 0 ? "normal field" : "jumped enemy");
        v_move = v_move + 3;
    }

    return;
}