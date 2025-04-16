#include <ncurses.h>

int main() {
    printw("Hello, ncurses!"); // Print text to the screen
    refresh();              // Refresh the screen
    getch();                // Wait for user input
    endwin();               // End ncurses mode
    return 0;
}