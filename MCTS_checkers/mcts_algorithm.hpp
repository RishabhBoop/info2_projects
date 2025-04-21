#ifndef MCTS_ALGORITHM_HPP
#define MCTS_ALGORITHM_HPP

#include "classes.hpp"

using namespace std;

/**
 * @brief Selects the child node with the highest UCB rating.
 *
 * Iterates through the children of the given node and returns the one with the maximum rating.
 * If the node has no children, it will return the node itself.
 * @param root_node The parent node whose children are to be evaluated.
 * @return Pointer to the child node with the highest UCB rating or the input node if no children.
 */
MCTS_leaf *select_best_child(MCTS_leaf*);

/**
 * @brief Performs the selection phase of the MCTS algorithm.
 *
 * Recursively traverses the tree starting from the root, always choosing the child
 * with the highest UCB rating (using `select_best_child`) until a leaf node
 * (a node with no children) is reached.
 * @param root The starting node for the selection process (usually the tree root).
 * @return Pointer to the selected leaf node.
 */
MCTS_leaf *selection(MCTS_leaf*);

/**
 * @brief Performs the expansion phase of the MCTS algorithm.
 *
 * If the selected node represents a non-terminal game state and has possible moves,
 * it randomly chooses one of the possible moves, creates a new child node representing
 * the resulting game state, adds this child to the selected node's children, and returns the new child.
 * If the game state is terminal or has no moves, it returns nullptr.
 *
 * @param root_node The leaf node selected during the selection phase.
 * @return Pointer to the newly created child node, or nullptr if expansion is not possible.
 */
MCTS_leaf *expansion(MCTS_leaf*);

/**
 * @brief Performs the simulation (playout) phase of the MCTS algorithm.
 *
 * Starting from the game state of the given leaf node, it simulates a complete game
 * by repeatedly choosing random moves for the current player until a terminal state is reached.
 * The simulation does not modify the MCTS tree itself.
 *
 * @param leaf_node The node from which to start the simulation (usually the node added during expansion).
 * @return int The result of the simulated game: PLAYER1 (1) if Player 1 wins, PLAYER2 (2) if Player 2 wins,
 *         or NOPLAYER (0) for a draw (as determined by `GameState::TerminalState`).
 */
int simulation(MCTS_leaf*);

/**
 * @brief Performs the backpropagation phase of the MCTS algorithm.
 *
 * Updates the statistics (wins and total games played) of the nodes along the path
 * from the given leaf node back up to the root of the MCTS tree.
 * The `total_games` count is incremented for each node in the path.
 * The `wins` count is incremented only if the simulation result matches the player who *made* the move leading to that node 
 *
 * @param leaf_node The node from which the simulation was run (usually the node added during expansion).
 * @param result The result of the simulation (1 for P1 win, 2 for P2 win, 0 for draw).
 */
void backpropagation(MCTS_leaf*, int);

// /**
//  * @brief Updates the UCB rating for all nodes in the subtree starting from the given node.
//  *
//  * Performs a Depth-First(DFS)-like traversal starting from `root_node`.
//  * For each node encountered, it calls `cal_rating()` to recalculate its UCB value based on its current win/loss stats and its parent's total games.
//  * @param root_node The root of the subtree whose ratings need updating.
//  * @note Implemented recursively.
//  */
// void update_rating(MCTS_leaf*);

/**
 * @brief Runs the complete MCTS process for a specified number of iterations.
 *
 * Each iteration consists of: Selection, Expansion, Simulation, Backpropagation, and Rating Update.
 * This function orchestrates the core MCTS loop to build and refine the search tree.
 *
 * @param root_node The root node of the MCTS tree.
 * @param num_iterations The number of MCTS iterations to perform.
 */
void train(MCTS_leaf*, int);

/**
 * @brief loads a leaf node from given input string
 * @param params The input string containing the leaf node data.
 * @param parent Pointer to the parent node (used to clone the game state); nullptr is root node.
 * @note The format of the input string should match the output format of save_leaf.
 */
MCTS_leaf* load_leaf(string params,MCTS_leaf*);

/**
 * @brief Saves the current state of the MCTS tree to a json file
 * The first node (root node) saves everything (gamestate, board, ...).
 * The rest of the nodes just save the moves (and other metadata), because the gamestate can then be reconstructed from the root node.
 * @param root_node The root node of the MCTS tree. 
 * @param out The output stream to write the tree data to.
 */
void save_tree(MCTS_leaf*,ofstream&);

/**
 * @brief loads tree from a file
 * @param string The whole, unfiltered input file as a string.
 * @return Pointer to the root node of the loaded tree.
 */
MCTS_leaf* load_tree(string);

/**
 * @brief deletes the whole tree, freeing up the used memory
 * @param root_node The root node of the MCTS tree.
 */
void destroy_tree(MCTS_leaf*);

/**
 * @brief Function to load a tree from a string.
 * Recursively reconstructs the MCTS tree from the string
 * @param root_node The parent node to which the new leaf will be added.
 * @param full_input The input string containing the leaf node data.
 * @throws runtime_error if any of the data is invalid.
 * @note This is a helper function for `load_tree`.
 * @return MCTS_leaf* 
 */
MCTS_leaf *load_tree_helper(MCTS_leaf*, string&);

/**
 * @brief Creates a board based on the given string.
 * @param choice defaults to "default" if no string is passed.
 * The string can be "default", "jump-test", "king-test", "win-test"
 * @throw runtime_error if the string is not valid and returns an empty board.
 * @return array<array<Piece, 8>, 8> 
 */
array<array<Piece, 8>, 8> create_board(string);

/**
 * @brief Clears the console screen.
 * Uses `clear` on Linux/macOS and `cls` on Windows.
 */
void clear_screen();

/**
 * @brief Function to compare two MCTS trees.
 * @param tree1 Pointer to the first MCTS tree.
 * @param tree2 Pointer to the second MCTS tree.
 * @throws runtime_error if the trees are not identical.
 * @note only used for testing purposes
 */
void compare_trees(MCTS_leaf*, MCTS_leaf*);

#endif