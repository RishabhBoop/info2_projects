#include <ncurses.h>
#include <iostream>

#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_RESET "\033[0m"

int main() {
    // Initialize ncurses
    initscr();              // Start ncurses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo input
    keypad(stdscr, TRUE);   // Enable special keys
    curs_set(0);            // Hide the cursor

    printw("This text is printed using ncurses.\n");
    printw("Press any key to see a line using ANSI escape codes.\n");
    refresh();

    getch(); // Wait for a key press

    // Use ANSI escape sequences with printf or std::cout
    endwin(); // End ncurses mode temporarily
    std::cout << ANSI_RED << "This line is red using ANSI escape codes.\n" << ANSI_RESET;
    std::cout << ANSI_GREEN << "This line is green using ANSI escape codes.\n" << ANSI_RESET;

    // Restore ncurses after using ANSI escape sequences
    initscr();
    printw("Back to ncurses mode. Press any key to exit.\n");
    refresh();

    getch(); // Wait for a key press
    endwin(); // End ncurses mode
    return 0;
}