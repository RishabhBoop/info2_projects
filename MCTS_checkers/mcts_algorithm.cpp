/**
 * @file mcts_algorithm.cpp
 * @author Rishabh Venugopal
 * @brief Implements the Monte Carlo Tree Search (MCTS) algorithm for Checkers.
 * This file contains the MCTS algorithm.
 * These are for the training of the AI.
 * selection -> expansion -> simulation -> backpropagation
 * @version 0.1
 * @date 2025-04-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "mcts_algorithm.hpp"

using namespace std;

MCTS_leaf *select_best_child(MCTS_leaf *root_node)
{
    while (root_node->children.size() != 0) // the root node has children, aka not at the end
    {
        // check all children and select the one with the highest UCB value
        double max_rating = -1;                      // initialize max rating
        MCTS_leaf *best_child = nullptr;             // initialize best child
        for (MCTS_leaf *child : root_node->children) // loop through all children
        {
            double calculated_rating = child->cal_rating(); // calculate the rating of the child
            if (calculated_rating > max_rating)
            {
                max_rating = calculated_rating;
                best_child = child;
            }
        }
        root_node = best_child;
    }
    return root_node;
}

MCTS_leaf *selection(MCTS_leaf *root)
{
    if (root == nullptr)
    {
        return root;
    }
    if (root->children.size() == 0) // if the root node has no children, return the root node
    {
        return root;
    }
    return selection(select_best_child(root));
}

MCTS_leaf *expansion(MCTS_leaf *root_node)
{
    // generate list of possible moves just to be sure
    root_node->state.list_all_possible_moves(root_node->state.get_current_player());
    // check if there are any possible moves
    int num_moves = root_node->state.possible_moves.size();
    // if the game is over, return nullptr
    if (root_node->state.TerminalState() != -1)
    {
        return nullptr;
    }
    // select a random move from the possible moves
    int random_move_index = rand() % num_moves;
    Move random_move = root_node->state.possible_moves.at(random_move_index);
    // create a new game state with the random move
    GameState new_game_state = root_node->state.clone();
    Board* tmp_board = new_game_state.get_board();
    random_move.perform_move(tmp_board, random_move);
    // change the player of the new game state
    new_game_state.switch_player();
    // populate the possible moves of the new game state
    new_game_state.list_all_possible_moves(new_game_state.get_current_player());
    // create a new child node with the new game state and add to the tree
    MCTS_leaf *new_child = new MCTS_leaf(new_game_state, random_move, root_node);
    root_node->children.push_back(new_child);
    return new_child;
}

int simulation(MCTS_leaf *leaf_node)
{
    // initialize a temporary game state
    GameState tmp_game_state = leaf_node->state.clone();
    // status of the game
    int status = tmp_game_state.TerminalState();
    // while the game is not over, keep playing by executing random moves until the game is over
    // each state is not saved on the tree
    while (status == -1)
    {
        // list all possible moves of the leaf node
        tmp_game_state.list_all_possible_moves(tmp_game_state.get_current_player());
        // check if there are any possible moves
        int num_moves = tmp_game_state.possible_moves.size();
        // if there are possible moves
        // create a new board and perform a random move on that
        if (num_moves > 0)
        {
            // select a random move from the possible moves
            int random_move_index = rand() % num_moves;
            Move random_move = tmp_game_state.possible_moves.at(random_move_index);
            // create a new game state with the random move
            GameState new_game_state = tmp_game_state.clone();
            Board *tmp_board = new_game_state.get_board();
            random_move.perform_move(tmp_board, random_move);
            // change the player of the new game state
            new_game_state.switch_player();
            // populate the possible moves of the new game state
            new_game_state.list_all_possible_moves(new_game_state.get_current_player());
            // set the new game state to the leaf node
            tmp_game_state = new_game_state;
        }
        // if there are no possible moves, the game is over
        status = tmp_game_state.TerminalState();
    }

    return status;
}

void backpropagation(MCTS_leaf *leaf_node, int result)
{
    // backpropagate the result to the root node
    MCTS_leaf *current_node = leaf_node;
    while (current_node != nullptr)
    {
        // update the total games and wins
        current_node->total_games++;
        if (result != NOPLAYER && current_node->state.get_current_player() != result)
        {
            // if the game is not a draw
            // if player 1 wins and the current player is player 2
            // or if player 2 wins and the current player is player 1
            // increment the wins of the current player
            current_node->wins++;
        }
        // move to the parent node
        current_node = current_node->parent;
    }
}

void train(MCTS_leaf *root_node, int num_iterations)
{
    // run the MCTS algorithm for num_iterations
    for (int i = 0; i <= num_iterations; i++)
    {
        // select
        MCTS_leaf *selected_node = selection(root_node);

        #ifdef DEBUG
        printf("Selected!\n");
        cout << "selected player: " << selected_node->state.get_current_player() << endl;
        #endif
        // if we are at the root node and it does not have any children, we need to explore every child
        if (selected_node == root_node && selected_node->children.size() == 0)
        {
            #ifdef DEBUG
            printf("At root node; exploring and simulating all children...\n");
            #endif
            selected_node->state.list_all_possible_moves(selected_node->state.get_current_player());
            // create all children states
            for (Move move : selected_node->state.possible_moves)
            {
                // create a new game state with the new move
                GameState new_game_state = selected_node->state.clone();
                Board *tmp_board = new_game_state.get_board();
                move.perform_move(tmp_board, move);
                // change the player of the new game state
                new_game_state.switch_player();
                // populate the possible moves of the new game state
                new_game_state.list_all_possible_moves(new_game_state.get_current_player());
                // create a new child node with the new game state and add to the tree
                MCTS_leaf *new_child = new MCTS_leaf(new_game_state, move, selected_node);
                selected_node->children.push_back(new_child);
            }
            #ifdef DEBUG
            printf("\tcreated all children!\n");
            #endif

            // mcts on each child node
            for (MCTS_leaf *child : selected_node->children)
            {
                if (child == nullptr)
                {
                    backpropagation(child, child->state.TerminalState());
                    continue;
                }
                // simulate the game from the child node
                int result = simulation(child);
                // backpropagate the result to the root node
                backpropagation(child, result);
            }
            #ifdef DEBUG
            printf("\tSimulated and Backpropagated all children!\n");
            #endif
        }
        else
        {
            #ifdef DEBUG
            printf("Not at root node; expanding and simulating...\n");
            #endif
            // expand selected node
            MCTS_leaf *expanded_node = expansion(selected_node);
            // if expanded_node is null, we are at a terminal state
            if (expanded_node == nullptr)
            {
                // backpropagate the result to the root node
                backpropagation(selected_node, selected_node->state.TerminalState());
                #ifdef DEBUG
                printf("\tExpanded node is null, aka terminal state is reached!\n");
                cout << "selected player: " << selected_node->state.get_current_player() << endl;
                cout << "terminal state: " << selected_node->state.TerminalState() << endl;
                #endif
                #ifdef DEBUG
                printf("----- Iteration %d complete -----\n", i);
                #endif
                continue;
            }
            // simulate the game from the expanded node
            int result = simulation(expanded_node);
            #ifdef DEBUG
            printf("\tSimulated!\n");
            #endif
            // backpropagate the result to the root node
            backpropagation(expanded_node, result);
            #ifdef DEBUG
            printf("\tBackpropagated!\n");
            #endif
        }
        // // update the rating of all of the nodes in the tree
        // update_rating(root_node);
        #ifdef DEBUG
        printf("----- Iteration %d complete -----\n", i);
        #endif
    }
    return;
}

void save_tree(MCTS_leaf *root_node, ofstream &out)
{
    // save the tree
    if (root_node == nullptr)
    {
        out << "#";
        return;
    }
    root_node->save_leaf(out);
    for (MCTS_leaf *child : root_node->children)
    {
        save_tree(child, out);
    }
    out << "$";
    return;
}

MCTS_leaf *load_tree_helper(MCTS_leaf *root_node, string &full_input)
{
    // check for null_pointers
    if (full_input[0] == '#')
    {
        full_input.erase(0, 1);
        return nullptr;
    }
    // check for end of child list -- this would be an error
    if (full_input[0] == '$')
    {
        throw runtime_error("Unexpected $ found.");
    }
    // verify beginning of string
    size_t bracket_start = full_input.find('[');
    if ((full_input[0] != 'r' && full_input[0] != 'c') || bracket_start == string::npos)
    {
        throw runtime_error("Unexpected character found. Expected r[ or c[.");
    }

    // find matching end bracket
    int tmp_counter = 1; // holds counter for bracket, so we can find matching bracket
    size_t bracket_end = string::npos;
    for (int i = bracket_start+1; i < full_input.length(); i++)
    {
        if (full_input[i] == '[')
        {
            tmp_counter++;
        }
        else if (full_input[i] == ']')
        {
            tmp_counter--;
        }
        if (tmp_counter == 0)
        {
            bracket_end = i;
            break;
        }
    }
    if (bracket_end == string::npos)
    {
        throw runtime_error("No matching end bracket found.");
    }

    /* we have found our data */
    // extract the data from the string
    string to_pass = full_input.substr(0, bracket_end + 1);
    // erase the data from the string
    full_input.erase(0, bracket_end + 1);
    // create new leaf

    MCTS_leaf *new_leaf = load_leaf(to_pass, root_node);
    new_leaf->parent = root_node;
    // create all of the children
    while (full_input[0] != '$')
    {
        // load the next child
        MCTS_leaf* child = load_tree_helper(new_leaf, full_input);
        if (child != nullptr)
        {
            new_leaf->children.push_back(child);
        }
    }
    // we have hit a $ (this should be the case)
    full_input.erase(0, 1); // erase the $

    // return the reconstructed leaf
    return new_leaf;
}

MCTS_leaf * load_tree(string full_input)
{
    MCTS_leaf* root_node = nullptr;
    try
    {
        root_node = load_tree_helper(root_node, full_input);
        return root_node;
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        // clean up the tree if one was created
        if (root_node != nullptr)
        {
            destroy_tree(root_node);
        }
        return nullptr;
    }
}

void destroy_tree(MCTS_leaf *root_node)
{
    if (root_node == nullptr)
    {
        return;
    }
    for (MCTS_leaf *child : root_node->children)
    {
        destroy_tree(child);
    }
    delete root_node;

    return;
}

array<array<Piece, 8>, 8> create_board(string choice)
{
    if (choice == "jump-test")
    {
        // Minimal board for testing jumps
        array<array<Piece, 8>, 8> m_board_j = {{{Piece(NOPLAYER, 0, 0), Piece(NOPLAYER, 1, 0), Piece(NOPLAYER, 2, 0), Piece(NOPLAYER, 3, 0), Piece(NOPLAYER, 4, 0), Piece(NOPLAYER, 5, 0), Piece(NOPLAYER, 6, 0), Piece(NOPLAYER, 7, 0)},
                                                {Piece(NOPLAYER, 0, 1), Piece(NOPLAYER, 1, 1), Piece(NOPLAYER, 2, 1), Piece(NOPLAYER, 3, 1), Piece(NOPLAYER, 4, 1), Piece(NOPLAYER, 5, 1), Piece(NOPLAYER, 6, 1), Piece(NOPLAYER, 7, 1)},
                                                {Piece(NOPLAYER, 0, 2), Piece(PLAYER1, 1, 2), Piece(NOPLAYER, 2, 2), Piece(NOPLAYER, 3, 2), Piece(NOPLAYER, 4, 2), Piece(NOPLAYER, 5, 2), Piece(NOPLAYER, 6, 2), Piece(NOPLAYER, 7, 2)},
                                                {Piece(NOPLAYER, 0, 3), Piece(NOPLAYER, 1, 3), Piece(PLAYER2, 2, 3), Piece(NOPLAYER, 3, 3), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 5, 3), Piece(NOPLAYER, 6, 3), Piece(NOPLAYER, 7, 3)},
                                                {Piece(NOPLAYER, 0, 4), Piece(NOPLAYER, 1, 4), Piece(NOPLAYER, 2, 4), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 5, 4), Piece(NOPLAYER, 6, 4), Piece(NOPLAYER, 7, 4)},
                                                {Piece(NOPLAYER, 0, 5), Piece(NOPLAYER, 1, 5), Piece(NOPLAYER, 2, 5), Piece(NOPLAYER, 3, 5), Piece(NOPLAYER, 4, 5), Piece(NOPLAYER, 5, 5), Piece(NOPLAYER, 6, 5), Piece(NOPLAYER, 7, 5)},
                                                {Piece(NOPLAYER, 0, 6), Piece(NOPLAYER, 1, 6), Piece(NOPLAYER, 2, 6), Piece(NOPLAYER, 3, 6), Piece(NOPLAYER, 4, 6), Piece(NOPLAYER, 5, 6), Piece(NOPLAYER, 6, 6), Piece(NOPLAYER, 7, 6)},
                                                {Piece(NOPLAYER, 0, 7), Piece(NOPLAYER, 1, 7), Piece(NOPLAYER, 2, 7), Piece(NOPLAYER, 3, 7), Piece(NOPLAYER, 4, 7), Piece(NOPLAYER, 5, 7), Piece(NOPLAYER, 6, 7), Piece(NOPLAYER, 7, 7)}}};
        return m_board_j;
    }
    else if (choice == "king-test")
    {
        // Minimal board for testing king-setting
        array<array<Piece, 8>, 8> m_board_k = {{{Piece(NOPLAYER, 0, 0), Piece(NOPLAYER, 1, 0), Piece(NOPLAYER, 2, 0), Piece(NOPLAYER, 3, 0), Piece(NOPLAYER, 4, 0), Piece(NOPLAYER, 5, 0), Piece(NOPLAYER, 6, 0), Piece(NOPLAYER, 7, 0)},
                                                {Piece(NOPLAYER, 0, 1), Piece(NOPLAYER, 1, 1), Piece(PLAYER2, 2, 1), Piece(NOPLAYER, 3, 1), Piece(NOPLAYER, 4, 1), Piece(NOPLAYER, 5, 1), Piece(NOPLAYER, 6, 1), Piece(NOPLAYER, 7, 1)}, // Player 2 piece on row 1 (index 1)
                                                {Piece(NOPLAYER, 0, 2), Piece(NOPLAYER, 1, 2), Piece(NOPLAYER, 2, 2), Piece(NOPLAYER, 3, 2), Piece(NOPLAYER, 4, 2), Piece(NOPLAYER, 5, 2), Piece(NOPLAYER, 6, 2), Piece(NOPLAYER, 7, 2)},
                                                {Piece(NOPLAYER, 0, 3), Piece(NOPLAYER, 1, 3), Piece(NOPLAYER, 2, 3), Piece(NOPLAYER, 3, 3), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 5, 3), Piece(NOPLAYER, 6, 3), Piece(NOPLAYER, 7, 3)},
                                                {Piece(NOPLAYER, 0, 4), Piece(NOPLAYER, 1, 4), Piece(NOPLAYER, 2, 4), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 5, 4), Piece(NOPLAYER, 6, 4), Piece(NOPLAYER, 7, 4)},
                                                {Piece(NOPLAYER, 0, 5), Piece(NOPLAYER, 1, 5), Piece(NOPLAYER, 2, 5), Piece(NOPLAYER, 3, 5), Piece(NOPLAYER, 4, 5), Piece(NOPLAYER, 5, 5), Piece(NOPLAYER, 6, 5), Piece(NOPLAYER, 7, 5)},
                                                {Piece(NOPLAYER, 0, 6), Piece(PLAYER1, 1, 6), Piece(NOPLAYER, 2, 6), Piece(NOPLAYER, 3, 6), Piece(NOPLAYER, 4, 6), Piece(NOPLAYER, 5, 6), Piece(NOPLAYER, 6, 6), Piece(NOPLAYER, 7, 6)}, // Player 1 piece on row 6 (index 6)
                                                {Piece(NOPLAYER, 0, 7), Piece(NOPLAYER, 1, 7), Piece(NOPLAYER, 2, 7), Piece(NOPLAYER, 3, 7), Piece(NOPLAYER, 4, 7), Piece(NOPLAYER, 5, 7), Piece(NOPLAYER, 6, 7), Piece(NOPLAYER, 7, 7)}}};
        return m_board_k;
    }
    else if (choice == "win-test")
    {
        // Minimal board for testing win/lose
        array<array<Piece, 8>, 8> m_board_w = {{{Piece(NOPLAYER, 0, 0), Piece(NOPLAYER, 1, 0), Piece(NOPLAYER, 2, 0), Piece(NOPLAYER, 3, 0), Piece(NOPLAYER, 4, 0), Piece(NOPLAYER, 5, 0), Piece(NOPLAYER, 6, 0), Piece(NOPLAYER, 7, 0)},
                                                {Piece(NOPLAYER, 0, 1), Piece(NOPLAYER, 1, 1), Piece(NOPLAYER, 2, 1), Piece(NOPLAYER, 3, 1), Piece(NOPLAYER, 4, 1), Piece(NOPLAYER, 5, 1), Piece(NOPLAYER, 6, 1), Piece(NOPLAYER, 7, 1)},
                                                {Piece(NOPLAYER, 0, 2), Piece(PLAYER1, 1, 2), Piece(NOPLAYER, 2, 2), Piece(NOPLAYER, 3, 2), Piece(NOPLAYER, 4, 2), Piece(NOPLAYER, 5, 2), Piece(NOPLAYER, 6, 2), Piece(NOPLAYER, 7, 2)},
                                                {Piece(NOPLAYER, 0, 3), Piece(NOPLAYER, 1, 3), Piece(PLAYER2, 2, 3), Piece(NOPLAYER, 3, 3), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 5, 3), Piece(NOPLAYER, 6, 3), Piece(NOPLAYER, 7, 3)},
                                                {Piece(NOPLAYER, 0, 4), Piece(NOPLAYER, 1, 4), Piece(NOPLAYER, 2, 4), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 5, 4), Piece(NOPLAYER, 6, 4), Piece(NOPLAYER, 7, 4)},
                                                {Piece(NOPLAYER, 0, 5), Piece(NOPLAYER, 1, 5), Piece(NOPLAYER, 2, 5), Piece(NOPLAYER, 3, 5), Piece(NOPLAYER, 4, 5), Piece(NOPLAYER, 5, 5), Piece(NOPLAYER, 6, 5), Piece(NOPLAYER, 7, 5)},
                                                {Piece(NOPLAYER, 0, 6), Piece(NOPLAYER, 1, 6), Piece(NOPLAYER, 2, 6), Piece(NOPLAYER, 3, 6), Piece(NOPLAYER, 4, 6), Piece(NOPLAYER, 5, 6), Piece(NOPLAYER, 6, 6), Piece(NOPLAYER, 7, 6)},
                                                {Piece(NOPLAYER, 0, 7), Piece(NOPLAYER, 1, 7), Piece(NOPLAYER, 2, 7), Piece(NOPLAYER, 3, 7), Piece(NOPLAYER, 4, 7), Piece(NOPLAYER, 5, 7), Piece(NOPLAYER, 6, 7), Piece(NOPLAYER, 7, 7)}}};
        return m_board_w;
    }
    else if (choice == "default")
    {
        // board[top to bottom][left to right] = board[y][x]; standard board
        array<array<Piece, 8>, 8> root_board = {{{Piece(NOPLAYER, 0, 0), Piece(PLAYER1, 1, 0), Piece(NOPLAYER, 2, 0), Piece(PLAYER1, 3, 0), Piece(NOPLAYER, 4, 0), Piece(PLAYER1, 5, 0), Piece(NOPLAYER, 6, 0), Piece(PLAYER1, 7, 0)},
                                                 {Piece(PLAYER1, 0, 1), Piece(NOPLAYER, 1, 1), Piece(PLAYER1, 2, 1), Piece(NOPLAYER, 3, 1), Piece(PLAYER1, 4, 1), Piece(NOPLAYER, 5, 1), Piece(PLAYER1, 6, 1), Piece(NOPLAYER, 7, 1)},
                                                 {Piece(NOPLAYER, 0, 2), Piece(PLAYER1, 1, 2), Piece(NOPLAYER, 2, 2), Piece(PLAYER1, 3, 2), Piece(NOPLAYER, 4, 2), Piece(PLAYER1, 5, 2), Piece(NOPLAYER, 6, 2), Piece(PLAYER1, 7, 2)},
                                                 {Piece(NOPLAYER, 0, 3), Piece(NOPLAYER, 1, 3), Piece(NOPLAYER, 2, 3), Piece(NOPLAYER, 3, 3), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 5, 3), Piece(NOPLAYER, 6, 3), Piece(NOPLAYER, 7, 3)},
                                                 {Piece(NOPLAYER, 0, 4), Piece(NOPLAYER, 1, 4), Piece(NOPLAYER, 2, 4), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 5, 4), Piece(NOPLAYER, 6, 4), Piece(NOPLAYER, 7, 4)},
                                                 {Piece(PLAYER2, 0, 5), Piece(NOPLAYER, 1, 5), Piece(PLAYER2, 2, 5), Piece(NOPLAYER, 3, 5), Piece(PLAYER2, 4, 5), Piece(NOPLAYER, 5, 5), Piece(PLAYER2, 6, 5), Piece(NOPLAYER, 7, 5)},
                                                 {Piece(NOPLAYER, 0, 6), Piece(PLAYER2, 1, 6), Piece(NOPLAYER, 2, 6), Piece(PLAYER2, 3, 6), Piece(NOPLAYER, 4, 6), Piece(PLAYER2, 5, 6), Piece(NOPLAYER, 6, 6), Piece(PLAYER2, 7, 6)},
                                                 {Piece(PLAYER2, 0, 7), Piece(NOPLAYER, 1, 7), Piece(PLAYER2, 2, 7), Piece(NOPLAYER, 3, 7), Piece(PLAYER2, 4, 7), Piece(NOPLAYER, 5, 7), Piece(PLAYER2, 6, 7), Piece(NOPLAYER, 7, 7)}}};

        return root_board;
    }
    else
    {
        throw runtime_error("Invalid board choice.");
        return {};
    }
}

void clear_screen()
{
    if (OS_LINUX)
    {
        (void)system("clear"); // signifies that i am ignoring the return value
    }
    else if (OS_LINUX == 0)
    {
        (void)system("cls"); // signifies that i am ignoring the return value
    }
    return;
}