#include "state.hpp"

using namespace std;

int main()
{
    // board[top to bottom][left to right] = board[y][x]; standard board
    array<array<Piece, 8>, 8> board = {{{Piece(NOPLAYER, 0, 0), Piece(PLAYER1, 1, 0), Piece(NOPLAYER, 2, 0), Piece(PLAYER1, 3, 0), Piece(NOPLAYER, 4, 0), Piece(PLAYER1, 5, 0), Piece(NOPLAYER, 6, 0), Piece(PLAYER1, 7, 0)},
                                        {Piece(PLAYER1, 0, 1), Piece(NOPLAYER, 1, 1), Piece(PLAYER1, 2, 1), Piece(NOPLAYER, 3, 1), Piece(PLAYER1, 4, 1), Piece(NOPLAYER, 5, 1), Piece(PLAYER1, 6, 1), Piece(NOPLAYER, 7, 1)},
                                        {Piece(NOPLAYER, 0, 2), Piece(PLAYER1, 1, 2), Piece(NOPLAYER, 2, 2), Piece(PLAYER1, 3, 2), Piece(NOPLAYER, 4, 2), Piece(PLAYER1, 5, 2), Piece(NOPLAYER, 6, 2), Piece(PLAYER1, 7, 2)},
                                        {Piece(NOPLAYER, 0, 3), Piece(NOPLAYER, 1, 3), Piece(NOPLAYER, 2, 3), Piece(NOPLAYER, 3, 3), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 5, 3), Piece(NOPLAYER, 6, 3), Piece(NOPLAYER, 7, 3)},
                                        {Piece(NOPLAYER, 0, 4), Piece(NOPLAYER, 1, 4), Piece(NOPLAYER, 2, 4), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 5, 4), Piece(NOPLAYER, 6, 4), Piece(NOPLAYER, 7, 4)},
                                        {Piece(PLAYER2, 0, 5), Piece(NOPLAYER, 1, 5), Piece(PLAYER2, 2, 5), Piece(NOPLAYER, 3, 5), Piece(PLAYER2, 4, 5), Piece(NOPLAYER, 5, 5), Piece(PLAYER2, 6, 5), Piece(NOPLAYER, 7, 5)},
                                        {Piece(NOPLAYER, 0, 6), Piece(PLAYER2, 1, 6), Piece(NOPLAYER, 2, 6), Piece(PLAYER2, 3, 6), Piece(NOPLAYER, 4, 6), Piece(PLAYER2, 5, 6), Piece(NOPLAYER, 6, 6), Piece(PLAYER2, 7, 6)},
                                        {Piece(PLAYER2, 0, 7), Piece(NOPLAYER, 1, 7), Piece(PLAYER2, 2, 7), Piece(NOPLAYER, 3, 7), Piece(PLAYER2, 4, 7), Piece(NOPLAYER, 5, 7), Piece(PLAYER2, 6, 7), Piece(NOPLAYER, 7, 7)}}};

    // Minimal board for testing jumps
    array<array<Piece, 8>, 8> m_board_j = {{{Piece(NOPLAYER, 0, 0), Piece(NOPLAYER, 1, 0), Piece(NOPLAYER, 2, 0), Piece(NOPLAYER, 3, 0), Piece(NOPLAYER, 4, 0), Piece(NOPLAYER, 5, 0), Piece(NOPLAYER, 6, 0), Piece(NOPLAYER, 7, 0)},
                                            {Piece(NOPLAYER, 0, 1), Piece(NOPLAYER, 1, 1), Piece(NOPLAYER, 2, 1), Piece(NOPLAYER, 3, 1), Piece(NOPLAYER, 4, 1), Piece(NOPLAYER, 5, 1), Piece(NOPLAYER, 6, 1), Piece(NOPLAYER, 7, 1)},
                                            {Piece(NOPLAYER, 0, 2), Piece(PLAYER1, 1, 2), Piece(NOPLAYER, 2, 2), Piece(NOPLAYER, 3, 2), Piece(NOPLAYER, 4, 2), Piece(NOPLAYER, 5, 2), Piece(NOPLAYER, 6, 2), Piece(NOPLAYER, 7, 2)},
                                            {Piece(NOPLAYER, 0, 3), Piece(NOPLAYER, 1, 3), Piece(PLAYER2, 2, 3), Piece(PLAYER2, 3, 3), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 5, 3), Piece(NOPLAYER, 6, 3), Piece(NOPLAYER, 7, 3)},
                                            {Piece(NOPLAYER, 0, 4), Piece(NOPLAYER, 1, 4), Piece(NOPLAYER, 2, 4), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 5, 4), Piece(NOPLAYER, 6, 4), Piece(NOPLAYER, 7, 4)},
                                            {Piece(NOPLAYER, 0, 5), Piece(NOPLAYER, 1, 5), Piece(NOPLAYER, 2, 5), Piece(NOPLAYER, 3, 5), Piece(NOPLAYER, 4, 5), Piece(NOPLAYER, 5, 5), Piece(NOPLAYER, 6, 5), Piece(NOPLAYER, 7, 5)},
                                            {Piece(NOPLAYER, 0, 6), Piece(NOPLAYER, 1, 6), Piece(NOPLAYER, 2, 6), Piece(NOPLAYER, 3, 6), Piece(NOPLAYER, 4, 6), Piece(NOPLAYER, 5, 6), Piece(NOPLAYER, 6, 6), Piece(NOPLAYER, 7, 6)},
                                            {Piece(NOPLAYER, 0, 7), Piece(NOPLAYER, 1, 7), Piece(NOPLAYER, 2, 7), Piece(NOPLAYER, 3, 7), Piece(NOPLAYER, 4, 7), Piece(NOPLAYER, 5, 7), Piece(NOPLAYER, 6, 7), Piece(NOPLAYER, 7, 7)}}};

    // Minimal board for testing king-setting
    array<array<Piece, 8>, 8> m_board_k = {{{Piece(NOPLAYER, 0, 0), Piece(NOPLAYER, 1, 0), Piece(NOPLAYER, 2, 0), Piece(NOPLAYER, 3, 0), Piece(NOPLAYER, 4, 0), Piece(NOPLAYER, 5, 0), Piece(NOPLAYER, 6, 0), Piece(NOPLAYER, 7, 0)},
                                            {Piece(NOPLAYER, 0, 1), Piece(NOPLAYER, 1, 1), Piece(PLAYER2, 2, 1), Piece(NOPLAYER, 3, 1), Piece(NOPLAYER, 4, 1), Piece(NOPLAYER, 5, 1), Piece(NOPLAYER, 6, 1), Piece(NOPLAYER, 7, 1)}, // Player 2 piece on row 1 (index 1)
                                            {Piece(NOPLAYER, 0, 2), Piece(NOPLAYER, 1, 2), Piece(NOPLAYER, 2, 2), Piece(NOPLAYER, 3, 2), Piece(NOPLAYER, 4, 2), Piece(NOPLAYER, 5, 2), Piece(NOPLAYER, 6, 2), Piece(NOPLAYER, 7, 2)},
                                            {Piece(NOPLAYER, 0, 3), Piece(NOPLAYER, 1, 3), Piece(NOPLAYER, 2, 3), Piece(NOPLAYER, 3, 3), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 5, 3), Piece(NOPLAYER, 6, 3), Piece(NOPLAYER, 7, 3)},
                                            {Piece(NOPLAYER, 0, 4), Piece(NOPLAYER, 1, 4), Piece(NOPLAYER, 2, 4), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 5, 4), Piece(NOPLAYER, 6, 4), Piece(NOPLAYER, 7, 4)},
                                            {Piece(NOPLAYER, 0, 5), Piece(NOPLAYER, 1, 5), Piece(NOPLAYER, 2, 5), Piece(NOPLAYER, 3, 5), Piece(NOPLAYER, 4, 5), Piece(NOPLAYER, 5, 5), Piece(NOPLAYER, 6, 5), Piece(NOPLAYER, 7, 5)},
                                            {Piece(NOPLAYER, 0, 6), Piece(PLAYER1, 1, 6), Piece(NOPLAYER, 2, 6), Piece(NOPLAYER, 3, 6), Piece(NOPLAYER, 4, 6), Piece(NOPLAYER, 5, 6), Piece(NOPLAYER, 6, 6), Piece(NOPLAYER, 7, 6)}, // Player 1 piece on row 6 (index 6)
                                            {Piece(NOPLAYER, 0, 7), Piece(NOPLAYER, 1, 7), Piece(NOPLAYER, 2, 7), Piece(NOPLAYER, 3, 7), Piece(NOPLAYER, 4, 7), Piece(NOPLAYER, 5, 7), Piece(NOPLAYER, 6, 7), Piece(NOPLAYER, 7, 7)}}};

    // Minimal board for testing win/lose
    array<array<Piece, 8>, 8> m_board_w = {{{Piece(NOPLAYER, 0, 0), Piece(NOPLAYER, 1, 0), Piece(NOPLAYER, 2, 0), Piece(NOPLAYER, 3, 0), Piece(NOPLAYER, 4, 0), Piece(NOPLAYER, 5, 0), Piece(NOPLAYER, 6, 0), Piece(NOPLAYER, 7, 0)},
                                            {Piece(NOPLAYER, 0, 1), Piece(NOPLAYER, 1, 1), Piece(NOPLAYER, 2, 1), Piece(NOPLAYER, 3, 1), Piece(NOPLAYER, 4, 1), Piece(NOPLAYER, 5, 1), Piece(NOPLAYER, 6, 1), Piece(NOPLAYER, 7, 1)},
                                            {Piece(NOPLAYER, 0, 2), Piece(PLAYER1, 1, 2), Piece(NOPLAYER, 2, 2), Piece(NOPLAYER, 3, 2), Piece(NOPLAYER, 4, 2), Piece(NOPLAYER, 5, 2), Piece(NOPLAYER, 6, 2), Piece(NOPLAYER, 7, 2)},
                                            {Piece(NOPLAYER, 0, 3), Piece(NOPLAYER, 1, 3), Piece(PLAYER2, 2, 3), Piece(NOPLAYER, 3, 3), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 5, 3), Piece(NOPLAYER, 6, 3), Piece(NOPLAYER, 7, 3)},
                                            {Piece(NOPLAYER, 0, 4), Piece(NOPLAYER, 1, 4), Piece(NOPLAYER, 2, 4), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 5, 4), Piece(NOPLAYER, 6, 4), Piece(NOPLAYER, 7, 4)},
                                            {Piece(NOPLAYER, 0, 5), Piece(NOPLAYER, 1, 5), Piece(NOPLAYER, 2, 5), Piece(NOPLAYER, 3, 5), Piece(NOPLAYER, 4, 5), Piece(NOPLAYER, 5, 5), Piece(NOPLAYER, 6, 5), Piece(NOPLAYER, 7, 5)},
                                            {Piece(NOPLAYER, 0, 6), Piece(NOPLAYER, 1, 6), Piece(NOPLAYER, 2, 6), Piece(NOPLAYER, 3, 6), Piece(NOPLAYER, 4, 6), Piece(NOPLAYER, 5, 6), Piece(NOPLAYER, 6, 6), Piece(NOPLAYER, 7, 6)},
                                            {Piece(NOPLAYER, 0, 7), Piece(NOPLAYER, 1, 7), Piece(NOPLAYER, 2, 7), Piece(NOPLAYER, 3, 7), Piece(NOPLAYER, 4, 7), Piece(NOPLAYER, 5, 7), Piece(NOPLAYER, 6, 7), Piece(NOPLAYER, 7, 7)}}};

    GameState init(Board(board), 1);
    GameState test_jumps(Board(m_board_j), 1);
    GameState test_king(Board(m_board_k), 1);
    GameState test_win(Board(m_board_w), 1);

    init.get_board().print_Board();
    return 0;
}