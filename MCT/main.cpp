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

    // // board[top to bottom][left to right] = board[y][x]; standard board
    // field board[8][8] = {
    //     {field(0, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 1, true, PLAYER1, PLAYER1_COLOR),      field(0, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 3, true, PLAYER1, PLAYER1_COLOR),      field(0, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 5, true, PLAYER1, PLAYER1_COLOR),      field(0, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 7, true, PLAYER1, PLAYER1_COLOR)},
    //     {field(1, 0, true, PLAYER1, PLAYER1_COLOR),      field(1, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 2, true, PLAYER1, PLAYER1_COLOR),      field(1, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 4, true, PLAYER1, PLAYER1_COLOR),      field(1, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 6, true, PLAYER1, PLAYER1_COLOR),      field(1, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(2, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 1, true, PLAYER1, PLAYER1_COLOR),      field(2, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 3, true, PLAYER1, PLAYER1_COLOR),      field(2, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 5, true, PLAYER1, PLAYER1_COLOR),      field(2, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 7, true, PLAYER1, PLAYER1_COLOR)},
    //     {field(3, 0, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(3, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 2, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(3, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 4, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(3, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 6, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(3, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(4, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 1, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(4, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 3, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(4, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 5, true, NOPLAYER, EMPTY_FIELD_COLOR),  field(4, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(5, 0, true, PLAYER2, PLAYER2_COLOR),      field(5, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 2, true, PLAYER2, PLAYER2_COLOR),      field(5, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 4, true,PLAYER2, PLAYER2_COLOR),       field(5, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 6, true, PLAYER2, PLAYER2_COLOR),      field(5, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(6, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 1, true, PLAYER2, PLAYER2_COLOR),      field(6, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 3, true, PLAYER2, PLAYER2_COLOR),      field(6, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 5, true, PLAYER2, PLAYER2_COLOR),      field(6, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 7, true, PLAYER2, PLAYER2_COLOR)},
    //     {field(7, 0, true, PLAYER2, PLAYER2_COLOR),      field(7, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 2, true, PLAYER2, PLAYER2_COLOR),      field(7, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 4, true, PLAYER2, PLAYER2_COLOR),      field(7, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 6, true, PLAYER2, PLAYER2_COLOR),      field(7, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)}
    // };

    // Minimal board for testing jumps
    field board[8][8] = {
        {field(0, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(1, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(2, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 1, true, PLAYER1, PLAYER1_COLOR), field(2, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(3, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 2, true, PLAYER2, PLAYER2_COLOR), field(3, 3, false, PLAYER2, PLAYER2_COLOR), field(3, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(4, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(5, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(6, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
        {field(7, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)}
    };

    // // Minimal board for testing king-setting
    // field board[8][8] = {
    //     // Initialize all fields as empty first
    //     {field(0, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(1, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 2, true, PLAYER2, PLAYER2_COLOR), field(1, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)}, // Player 1 piece on row 1 (index 1)
    //     {field(2, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(3, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(4, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(5, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(6, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 1, true, PLAYER1, PLAYER1_COLOR), field(6, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)}, // Player 2 piece on row 6 (index 6)
    //     {field(7, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)}
    // };

    // // Minimal board for testing win/lose
    // field board[8][8] = {
    //     {field(0, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(0, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(1, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(1, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(2, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 1, true, PLAYER1, PLAYER1_COLOR), field(2, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(2, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(3, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 2, true, PLAYER2, PLAYER2_COLOR), field(3, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(3, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(4, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(4, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(5, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(5, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(6, 0, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 1, true, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 2, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 3, true, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 4, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 5, true, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 6, false, NOPLAYER, EMPTY_FIELD_COLOR), field(6, 7, true, NOPLAYER, EMPTY_FIELD_COLOR)},
    //     {field(7, 0, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 1, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 2, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 3, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 4, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 5, false, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 6, true, NOPLAYER, EMPTY_FIELD_COLOR), field(7, 7, false, NOPLAYER, EMPTY_FIELD_COLOR)}
    // };

    print_board(board);
    // ask which player to play
    cout << "Welcome to Checkers!" << endl;
    printf("Who do you want to play as? ");
    printf("Player 1 (1), Player 2 (2): ");
    int player;
    cin >> player;
    if (player != 1 && player != 2)
    {
        cout << "invalid input" << endl;
        return 0;
    }

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
            // check if player is allowed to select the field
            if (board[y][x].myPiece->player != player)
            {
                cout << "invalid input: wrong player" << endl;
                continue;
            }
            board[y][x].select();
        }
        else
        {
            cout << "invalid input" << endl;
            continue;
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
            cout << "select field [y]: ";
            cin >> y_usr;
            cout << "select field [x]: ";
            cin >> x_usr;
            int x_m = x_usr - 1;
            int y_m = y_usr - 1;
            /* Check if y value and x value are in list of possible moves */
            bool found = false;
            for (int i = 0; i < possible_moves.size(); i+=5)
            {
                if (possible_moves[i] == y_m && possible_moves[i+1] == x_m)
                {
                    // perform move
                    found = true;
                    board[y][x].move(y_m, x_m, board, possible_moves[i+2], possible_moves[i+3], possible_moves[i+4]);
                    printf("moved!\n");
                    // select and print new field
                    board[y_m][x_m].unselect();
                    print_board(board);
                    board[y_m][x_m].print_field();
                }
            }
            // if the move was not found in the list of possible moves
            if (!found)
            {
                // unselect the field and print an error message
                cout << "invalid move: wrong coordinate" << endl;
                board[y][x].unselect();
            }
        }
        else if (input == "unselect" || input == "u")
        {
            board[y][x].unselect();
        }
        else
        {
            cout << "invalid input: wrong coordinate while unselecting" << endl;
        }
        // check if player 1 has lost
        if (has_lost(1))
        {
            cout << "Player 1 has lost!" << endl;
            quit = true;
        }
        // check if player 2 has lost
        if (has_lost(2))
        {
            cout << "Player 2 has lost!" << endl;
            quit = true;
        }
    }
    return 0;
}