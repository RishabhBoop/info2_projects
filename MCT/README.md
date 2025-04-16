# Documentation for `field.cpp`

This document provides detailed documentation for the C++ source file `field.cpp`, which defines the `field` class and related utility functions for managing a checkers game board.

---

## Overview

`field.cpp` implements the core logic for representing and manipulating individual squares (fields) on an 8x8 checkers board. The `field` class encapsulates the state of a square, including its position, the piece occupying it (if any), and its visual representation. Several utility functions are also provided for board operations like checking bounds, validating moves, printing the board, and clearing the console.

---

## Class: `field`

Represents a single square on the game board.

### Member Variables (Implicit via `piece* myPiece`)

The `field` class contains a pointer `myPiece` to a `piece` object (defined in `field.hpp`). The `piece` object holds the actual state information:

-   **`player`**: `int` - The player occupying the field (`NOPLAYER` (0), `PLAYER1` (1), or `PLAYER2` (2)).
-   **`is_black`**: `bool` - Indicates if the underlying board square is black (used for board pattern, not piece color).
-   **`is_king`**: `bool` - `true` if the piece on this field is a king, `false` otherwise.
-   **`color`**: `const char*` - A C-style string representing the ANSI color code for displaying the piece/field.
-   **`selected`**: `const char*` - A C-style string representing the ANSI background color code when the field is selected.

### Member Variables (Direct)

-   **`x`**: `int` - The column index (0-7, left to right) of the field.
-   **`y`**: `int` - The row index (0-7, top to bottom) of the field.

### Methods

#### Constructor: `field::field`

```cpp
field(const int y, const int x, const bool is_black, const int player, const char *color, bool is_king = false, const char *selected = NOT_SELECTED_FIELD_COLOR);
```

-   **Purpose**: Constructs a new `field` object and initializes its associated `piece`.
-   **Parameters**:
    -   `y`: `const int` - The row index (0-7).
    -   `x`: `const int` - The column index (0-7).
    -   `is_black`: `const bool` - `true` if the underlying board square is black, `false` otherwise.
    -   `player`: `const int` - The player occupying the field (`NOPLAYER`, `PLAYER1`, or `PLAYER2`).
    -   `color`: `const char*` - The display color code for the piece/field.
    -   `is_king`: `bool` (optional, default: `false`) - `true` if the piece is a king.
    -   `selected`: `const char*` (optional, default: `NOT_SELECTED_FIELD_COLOR`) - The background color code when selected.
-   **Output**: None. Initializes the `field` object's state.

#### `field::print_field`

```cpp
void print_field();
```

-   **Purpose**: Prints the detailed state of the *current* field object to standard output, including coordinates, player, king status, color codes, etc. Useful for debugging.
-   **Parameters**: None.
-   **Output**: Prints formatted field details to the console.

#### `field::in_bounds`

```cpp
bool in_bounds(int row, int col);
```

-   **Purpose**: Checks if the given row and column indices are within the valid board boundaries (0-7).
-   **Parameters**:
    -   `row`: `int` - The row index to check.
    -   `col`: `int` - The column index to check.
-   **Returns**: `bool` - `true` if `0 <= row <= 7` and `0 <= col <= 7`, `false` otherwise.

#### `field::is_empty`

```cpp
bool is_empty(field obj);
```

-   **Purpose**: Checks if the provided `field` object is empty (i.e., its piece belongs to `NOPLAYER`).
-   **Parameters**:
    -   `obj`: `field` - The field object to check.
-   **Returns**: `bool` - `true` if the field's piece player is `NOPLAYER` (0), `false` otherwise.

#### `field::list_possible_moves`

```cpp
void list_possible_moves(field board[8][8], vector<int> &possible_moves);
```

-   **Purpose**: Calculates all valid moves (simple moves and jumps) for the piece currently on *this* field, considering player direction and king status. Appends the found moves to the provided vector.
-   **Parameters**:
    -   `board`: `field[8][8]` - The current 8x8 game board state.
    -   `possible_moves`: `vector<int>&` - A reference to a vector where possible moves will be stored.
-   **Output**: Modifies the `possible_moves` vector. Each logical move is stored as a sequence of 5 integers:
    -   `dest_y`: Destination row index.
    -   `dest_x`: Destination column index.
    -   `move_type`: `0` for a normal move, `1` for a jump.
    -   `enemy_y`: Row index of the jumped enemy piece (`-1` if not a jump).
    -   `enemy_x`: Column index of the jumped enemy piece (`-1` if not a jump).

#### `field::select`

```cpp
void select();
```

-   **Purpose**: Marks *this* field as selected by changing its piece's background and foreground color codes for display.
-   **Parameters**: None.
-   **Output**: Modifies the `myPiece->selected` and `myPiece->color` attributes of the field.

#### `field::unselect`

```cpp
void unselect();
```

-   **Purpose**: Resets the selection state of *this* field, restoring its piece's original display color codes.
-   **Parameters**: None.
-   **Output**: Modifies the `myPiece->selected` and `myPiece->color` attributes of the field.

#### `field::move`

```cpp
void move(int dest_y, int dest_x, field board[8][8], bool jump, int enemy_y, int enemy_x);
```

-   **Purpose**: Executes a move by transferring the piece from *this* field to the specified destination field on the board. Handles removing jumped enemy pieces and king promotion.
-   **Parameters**:
    -   `dest_y`: `int` - The destination row index.
    -   `dest_x`: `int` - The destination column index.
    -   `board`: `field[8][8]` - The game board array, which will be modified.
    -   `jump`: `bool` - `true` if this move is a jump, `false` otherwise.
    -   `enemy_y`: `int` - The row index of the jumped enemy piece (used only if `jump` is `true`, otherwise ignored, typically `-1`).
    -   `enemy_x`: `int` - The column index of the jumped enemy piece (used only if `jump` is `true`, otherwise ignored, typically `-1`).
-   **Output**: Modifies the `board` state:
    -   The piece from the original field (`this`) is moved to `board[dest_y][dest_x]`.
    -   The original field (`this`) becomes empty (`NOPLAYER`).
    -   If `jump` is `true`, the enemy piece at `board[enemy_y][enemy_x]` is removed (set to `NOPLAYER`).
    -   Checks for and applies king promotion if the piece reaches the opponent's back rank.

---

## Utility Functions (Standalone)

These functions operate independently of a specific `field` object.

### `is_move_possible`

```cpp
bool is_move_possible(const vector<int>& possible_moves, int target_y, int target_x);
```

-   **Purpose**: Checks if a given target coordinate exists as a valid destination within a pre-calculated list of possible moves.
-   **Parameters**:
    -   `possible_moves`: `const vector<int>&` - The vector containing possible moves (generated by `field::list_possible_moves`).
    -   `target_y`: `int` - The target destination row index to check for.
    -   `target_x`: `int` - The target destination column index to check for.
-   **Returns**: `bool` - `true` if a move ending at (`target_y`, `target_x`) is found in the `possible_moves` vector, `false` otherwise.

### `clear_screen`

```cpp
void clear_screen();
```

-   **Purpose**: Clears the console screen using system-specific commands (`clear` for Linux/macOS, `cls` for Windows). Relies on the `OS_LINUX` macro definition.
-   **Parameters**: None.
-   **Output**: Clears the terminal display.

### `print_board`

```cpp
void print_board(field board[8][8]);
```

-   **Purpose**: Prints the entire 8x8 game board to the console using ANSI color codes defined in the `field` objects. Clears the screen before printing. Includes row and column numbers.
-   **Parameters**:
    -   `board`: `field[8][8]` - The 8x8 game board array containing the `field` objects to print.
-   **Output**: Prints a formatted representation of the game board to the console.

### `print_possible_moves`

```cpp
void print_possible_moves(int x, int y, vector<int> &possible_moves);
```

-   **Purpose**: Prints the list of possible moves (previously calculated for a specific field) to the console in a human-readable format.
-   **Parameters**:
    -   `x`: `int` - The column index (0-7) of the field whose moves are being printed (used for display context).
    -   `y`: `int` - The row index (0-7) of the field whose moves are being printed (used for display context).
    -   `possible_moves`: `vector<int>&` - The vector containing the possible moves (sequence of 5 integers per move) to be printed.
-   **Output**: Prints the possible moves, including destination coordinates, move type (normal/jump), and jumped enemy coordinates (if applicable), to the console.

### `has_lost`

```cpp
bool has_lost(int player);
```

-   **Purpose**: Checks if a given player has lost the game by having no pieces remaining on the board.
-   **Parameters**:
    -   `player`: `int` - The player ID to check (1 for PLAYER1, 2 for PLAYER2).
-   **Returns**: `bool` - `true` if the specified player has 0 pieces left (based on global counters `num_player1_pieces` or `num_player2_pieces`), `false` otherwise.
