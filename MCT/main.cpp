#include "field.hpp"

using namespace std;

/*
Try and implement checkers in the terminal
    8x8 board with alternating black and white fields
    only play on black fields
    12 pieces for each player
    2 players
    3 different types of moves:
        - move a piece diagonally
        - if diagonal is enemy and after enemy is empty: jump over an enemy piece
            - jump over multiple enemy pieces
    only move forward
    if a piece reaches the end of the board, it becomes a king and can move in all directions
*/



int main()
{
    clear_screen();
    printf("HELLO\n");
    if (OS_LINUX == 0) cout << BOLDRED << "If you are on Windows, the colors might not work properly." << RESET << endl;
    printf("Checkers\n");
    cout << PLAYER1_COLOR << "Player 1" << RESET << endl;
    cout << PLAYER2_COLOR << "Player 2" << RESET << endl;

    // board[top to bottom][left to right] = board[y][x]
    field board[8][8] = {
        {field(0, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 1, true, PLAYER1, PLAYER1_COLOR),      field(0, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 3, true, PLAYER1, PLAYER1_COLOR),      field(0, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 5, true, PLAYER1, PLAYER1_COLOR),      field(0, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 7, true, PLAYER1, PLAYER1_COLOR)},
        {field(1, 0, true, PLAYER1, PLAYER1_COLOR),      field(1, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 2, true, PLAYER1, PLAYER1_COLOR),      field(1, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 4, true, PLAYER1, PLAYER1_COLOR),      field(1, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 6, true, PLAYER1, PLAYER1_COLOR),      field(1, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(2, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 1, true, PLAYER1, PLAYER1_COLOR),      field(2, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 3, true, PLAYER1, PLAYER1_COLOR),      field(2, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 5, true, PLAYER1, PLAYER1_COLOR),      field(2, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 7, true, PLAYER1, PLAYER1_COLOR)},
        {field(3, 0, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(3, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 2, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(3, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 4, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(3, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 6, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(3, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(4, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 1, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(4, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 3, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(4, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 5, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(4, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(5, 0, true, PLAYER2, PLAYER2_COLOR),      field(5, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 2, true, PLAYER2, PLAYER2_COLOR),      field(5, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 4, true,PLAYER2, PLAYER2_COLOR),       field(5, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 6, true, PLAYER2, PLAYER2_COLOR),      field(5, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(6, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 1, true, PLAYER2, PLAYER2_COLOR),      field(6, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 3, true, PLAYER2, PLAYER2_COLOR),      field(6, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 5, true, PLAYER2, PLAYER2_COLOR),      field(6, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 7, true, PLAYER2, PLAYER2_COLOR)},
        {field(7, 0, true, PLAYER2, PLAYER2_COLOR),      field(7, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 2, true, PLAYER2, PLAYER2_COLOR),      field(7, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 4, true, PLAYER2, PLAYER2_COLOR),      field(7, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 6, true, PLAYER2, PLAYER2_COLOR),      field(7, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)}
    };
    
    print_board(board);
    bool quit = false;
    while(!quit)
    {
        printf("what do you want to do? ");
        printf("quit (q), select (s): ");
        int y_usr; // top to bottom
        int x_usr; // left to right
        int x;
        int y;
        bool usr_sel = false;
        // ask for input
        string input;
        cin >> input;
        // normalize input to lowercase
        for (auto c : input) c = tolower(c);
        if (input == "quit" || input == "q")
        {
            quit = true;
            break;
        }
        else if (input == "select" || input == "s")
        {
            cout << "select field [x]: ";
            cin >> x_usr;
            cout << "select field [y]: ";
            cin >> y_usr;
            x = x_usr - 1;
            y = y_usr - 1;
            board[y][x].select();
        }
        vector<int> possible_moves;
        print_board(board);
        board[y][x].print_field();
        board[y][x].list_possible_moves(board, possible_moves);
        printf("-----------------\n");
        print_possible_moves(x, y, possible_moves);
        printf("-----------------\n");


        // ask if the user wants to move or unselect
        input = "";
        cout << "move (m), unselect (u): ";
        cin >> input;
        // normalize input to lowercase
        for (auto c : input) c = tolower(c);
        if (input == "move" || input == "m")
        {
            // ask for input 
            cout << "select field [x]: ";
            cin >> x_usr;
            cout << "select field [y]: ";
            cin >> y_usr;
            x = x_usr - 1;
            y = y_usr - 1;
            // check if the field is in list of possible moves
            bool inList = false;
            while(true) 
            {
                
            }
        }

    }
    return 0;
}