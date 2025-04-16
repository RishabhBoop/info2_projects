# Documentation for `field.cpp`

This document provides an overview and explanation of the `field.cpp` file, which implements a `field` class and related functions for managing a game board. The code is written in C++ and is designed for intermediate programmers.

---

## Overview

The `field` class represents a single square on a game board. It includes attributes to track the position, ownership, and state of the square, as well as methods to manipulate and display the board. The file also contains utility functions for clearing the screen, printing the board, and listing possible moves.

---

## Class: `field`

### Attributes
- **`x`**: The column number (0-7) of the field.
- **`y`**: The row number (0-7) of the field.
- **`is_black`**: A boolean indicating if the field is black.
- **`player`**: An integer representing the player occupying the field (`0` for empty, `1` for Player 1, `2` for Player 2).
- **`color`**: A string representing the color of the field.
- **`is_king`**: A boolean indicating if the piece on the field is a king.
- **`selected`**: A string representing the selection state of the field.

### Methods

#### Constructor: `field`
Initializes a field with the given attributes:
```cpp
field(const int y, const int x, const bool is_black, const int player, const char *color, bool is_king, const char *selected);
```

Each parameter corresponds to an attribute of the `field` class:
- **`y`**: The row number of the field (0-7).
- **`x`**: The column number of the field (0-7).
- **`is_black`**: A boolean indicating whether the field is black (`true` for black, `false` for white).
- **`player`**: An integer representing the player occupying the field (`0` for empty, `1` for Player 1, `2` for Player 2).
- **`color`**: A C-style string representing the color of the field (e.g., "red", "black").
- **`is_king`**: A boolean indicating whether the piece on the field is a king (`true` for king, `false` otherwise).
- **`selected`**: A C-style string representing the selection state of the field (e.g., "selected", "unselected").

#### `print_field`
Prints the details of the current field, including its position, player, and state.

#### `tmp_print_any_field`
Prints the details of any given field object.

#### `in_bounds`
Checks if a given row and column are within the valid range of the board (0-7).

#### `is_empty`
Checks if a field is empty (i.e., not occupied by any player).

#### `list_possible_moves`
Calculates and lists all possible moves for the current field based on the game rules. It considers normal moves and jumps over enemy pieces.

#### `select`
Marks the field as selected and updates its color.

#### `unselect`
Unselects the field and resets its color based on its state.

#### `move`
Moves a piece from the current field to a destination field. If the move involves a jump, it removes the enemy piece.

---

## Utility Functions

### `clear_screen`
Clears the console screen. It uses `system("clear")` for Linux and `system("cls")` for other operating systems.

### `print_board`
Prints the entire game board in a human-readable format. Each field is displayed with its player and color.

### `print_board_ncurses` (Conditional)
Prints the board using the `ncurses` library if the `NCURSES` macro is defined.

### `print_possible_moves`
Prints all possible moves for a given field in a structured format.

## Example Usage

### Creating a Field
```cpp
field myField(0, 0, true, 1, "red", false, "selected");
```

### Printing a Field
```cpp
myField.print_field();
```

### Listing Possible Moves
```cpp
field board[8][8]; // Assume this is initialized
vector<int> possible_moves;
myField.list_possible_moves(board, possible_moves);
```

### Moving a Piece

To move a piece on the board, you can use the `move` method of the `field` class. This method updates the state of the board by moving a piece from the current field to a destination field. It also handles special cases like jumping over an enemy piece.

#### Syntax
```cpp
void move(int new_y, int new_x, field board[8][8], bool is_jump, int player, int enemy);
```

#### Parameters
- **`new_y`**: The row index of the destination field (0-7).
- **`new_x`**: The column index of the destination field (0-7).
- **`board`**: The 2D array representing the game board.
- **`is_jump`**: A boolean indicating whether the move is a jump over an enemy piece (`true` for jump, `false` otherwise).
- **`player`**: The integer representing the current player (e.g., `PLAYER1` or `PLAYER2`).
- **`enemy`**: The integer representing the opposing player.

#### Example Usage
```cpp
// Assume the current field is at (2, 3) and the destination is (4, 5)
int current_y = 2;
int current_x = 3;
int destination_y = 4;
int destination_x = 5;
bool is_jump = true; // This move involves jumping over an enemy piece
int player = PLAYER1;
int enemy = PLAYER2;

// Perform the move
board[current_y][current_x].move(destination_y, destination_x, board, is_jump, player, enemy);
```

#### Key Points
1. **Validation**: Before calling the `move` method, ensure the move is valid by checking the list of possible moves using the `list_possible_moves` method.
2. **Jumping**: If the move involves a jump, the `move` method will automatically remove the enemy piece from the board.
3. **State Update**: The method updates the state of both the source and destination fields, including attributes like `player` and `is_king`.
4. **King Promotion**: If a piece reaches the opposite end of the board, it is promoted to a king. This is handled within the `move` method.

#### Additional Notes
- The `move` method is a critical part of the game logic and ensures that all rules of the game are followed during a move.
- Always use the `print_board` function after a move to visually confirm the updated state of the board.

---

This documentation should help you understand the purpose and functionality of the `field.cpp` file. If you have further questions, feel free to ask!