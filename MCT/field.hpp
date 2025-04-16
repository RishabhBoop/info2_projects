#if defined(NCURSES)
#include <ncurses.h>
#endif

#ifndef FIELD_HPP
#define FIELD_HPP
#include <iostream>
#include <vector>
// #include <stdlib.h>         // https://stackoverflow.com/questions/902261/is-there-a-decent-wait-function-in-c
// #include <time.h>

#ifdef __GNUC__ 
#define OS_LINUX 1
// #include <unistd.h>
#elif defined(_WIN64)
#define OS_LINUX 0
// #include <windows.h>
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

class field
{
public:
    int x;
    int y;
    bool is_black;
    bool has_piece;
    int player;
    bool is_king;
    const char *color;
    const char *selected;

    field(const int y, const int x, const bool is_black, const int player, const char *color, bool is_king = false, const char *selected = NOT_SELECTED_FIELD_COLOR);

    bool is_empty(field obj);

    void print_field();

    void tmp_print_any_field(field obj);

    bool in_bounds(int row, int col);

    void list_possible_moves(field board[8][8], vector<int> &possible_moves);

    void select();

    void unselect();

    void move(int dest_y, int dest_x, field board[8][8], bool jump = false, int enemy_y = 0, int enemy_x = 0);
};
void clear_screen();
void print_board(field board[8][8]);
void print_board_ncurses(field board[8][8]);
void print_possible_moves(int x, int y, vector<int> &possible_moves);
#endif