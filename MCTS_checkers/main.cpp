/**
 * @author Rishabh Venugopal
 * @version 1.0
 * @date 2025-04-18
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "classes.hpp"
#include "mcts_algorithm.hpp"
#include <chrono>
#include <limits>

#ifdef TESTING
#include "tests.hpp"
#define TEST()                    \
    {                             \
        printf("Testing mode\n"); \
        return all_tests();       \
    }
#else
#define TEST()
#endif


using namespace std;

void print_prj_banner();
int choice1();
int choice2();
MCTS_leaf* select_most_visited_child(MCTS_leaf*);

int main()
{
    clear_screen();
    print_prj_banner();
    DEBUG_PRINT("Debug mode activated\n");
    TEST();
    // -------- User input: programm mode --------
    int choice;
    while (true)
    {
        printf("What would you like to do?\n");
        printf("1. Play a game\n");
        printf("2. Train the AI\n");
        printf("3. Exit\n");
        printf("--------------------\n");
        choice = 0;
        cin >> choice;
        if (cin.fail() || choice > LIMIT || choice < 1)
        {
            cin.clear();                                         // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            cout << "Invalid input, please try again.\n";
            continue;
        }
        break;
    }
    // --------- choice 1: play a game ---------
    if (choice == 1)
    {
        return choice1();
    }
    // --------- choice 2: train the AI ---------
    else if (choice == 2)
    {
        return choice2();
    }
    // --------- choice 4: only for debugging ---------
    else if(choice == 4)
    {
        MCTS_leaf *mcts_tree = nullptr;
        Board board = create_board("default");
        GameState game_state = GameState(board, PLAYER1);
        mcts_tree = new MCTS_leaf(game_state, Move(-1, -1, -1, -1, false, -1, -1), nullptr, {}, 0, 0, true, false);
        train(mcts_tree, 5);
        // traverse the tree and print the boards
        MCTS_leaf* current_node = mcts_tree;
        int i = 0;
        while (i<7)
        {
            current_node->state.get_board()->print_Board();
            cout << "Player " << current_node->state.get_current_player() << endl;
            current_node->print_move();
            for (MCTS_leaf* child : current_node->children)
            {
                DEBUG_PRINT("Child: ");
                DEBUG_PRINT(child->state.get_current_player());
                DEBUG_PRINT("\n");
                DEBUG_PRINT("Child move: ");
                child->print_move();
            }
            MCTS_leaf* newnode = select_most_visited_child(current_node);
            DEBUG_PRINT("New node selected!\n");
            DEBUG_PRINT("Newnode Player: ");
            DEBUG_PRINT(newnode->state.get_current_player());
            DEBUG_PRINT("\n");
            DEBUG_PRINT("newnode move: ");
            DEBUG_FUNC(newnode->print_move());
            DEBUG_PRINT("--------------------------------------\n");

            DEBUG_PRINT("\n");
            if (newnode == nullptr)
            {
                // chose a random child 
                DEBUG_PRINT("No children found, choosing a random child...\n");
                if (current_node->children.size() == 0)
                {
                    DEBUG_PRINT("No children found, exiting...\n");
                    current_node->state.get_board()->print_Board();
                    DEBUG_PRINT("nooo\n");
                    return 1;
                }
                current_node = current_node->children.at(0);
                i++; 
                continue;
            } 
            current_node = newnode;
            i++;
        }
        destroy_tree(mcts_tree);
        DEBUG_PRINT("Tree destroyed!\n");
        
    }
    // --------- choice 3: exit ---------
    else
    {
        cout << "exiting...\n";
        return 0;
    }

    return 0;
}

MCTS_leaf *select_most_visited_child(MCTS_leaf *root_node)
{
    if (root_node == nullptr || root_node->children.empty())
    {
        return root_node; // Return node itself if null or no children
    }

    MCTS_leaf *most_visited_child = nullptr;
    int max_visits = -1; // Initialize max visits to handle nodes with 0 visits correctly
    // Iterate through children to find the one with the most visits
    for (MCTS_leaf *child : root_node->children)
    {
        if (child->total_games > max_visits)
        {
            max_visits = child->total_games;
            most_visited_child = child;
        }
    }
    // // If no child was selected (e.g., all have 0 visits), return the first child as a default.
    // // This ensures we always return a child if children exist.
    // if (most_visited_child == nullptr && !root_node->children.empty()) {
    //      return root_node->children.front();
    // }

    return most_visited_child; // Return the child with the most visits
}

void print_prj_banner()
{
    cout << BOLDBLUE <<
        R"(
     ____  ____   ___      _ _____ _  _______   _           __  __  ____ _____ ____  
    |  _ \|  _ \ / _ \    | | ____| |/ /_   _| / |         |  \/  |/ ___|_   _/ ___| 
    | |_) | |_) | | | |_  | |  _| | ' /  | |   | |  _____  | |\/| | |     | | \___ \ 
    |  __/|  _ <| |_| | |_| | |___| . \  | |   | | |_____| | |  | | |___  | |  ___) |
    |_|   |_| \_\\___/ \___/|_____|_|\_\ |_|   |_|         |_|  |_|\____| |_| |____/ 
    
    )"
         << RESET << endl;
    return;
}

// ------ SAVES AND DESTROYS TRREE -----
int save_and_exit(MCTS_leaf* mcts_tree)
{
    ofstream output_file("mcts_tree.txt");
    if (output_file.is_open())
    {
        save_tree(mcts_tree, output_file);
        output_file.close();
    }
    else
    {
        cout << "Unable to open file\n";
        destroy_tree(mcts_tree);
        return 1;
    }
    DEBUG_PRINT("saved tree to file!\n");
    // destroy the tree
    destroy_tree(mcts_tree);
    DEBUG_PRINT("Tree destroyed!\n");

    return 0;
}

// ------ USER INTERACTION MODE -----
int choice1()
{
    // load the tree from file and reconstruct tree
    MCTS_leaf *mcts_tree;
    // if no tree is found, train the AI and save the tree to file and try again
    while (true)
    {
        ifstream input_file("mcts_tree.txt");
        if (input_file.is_open())
        {
            cout << "loading tree from file...\n";
            string raw_input;
            getline(input_file, raw_input);
            mcts_tree = load_tree(raw_input);
        }
        else
        {
            int res = choice2();
            if (res != 0) return res;
            cout << "let us try again...\n";
            continue;
        }
        break;
    }

    // we now have a tree, so we can play the game
    clear_screen();
    mcts_tree->state.get_board()->print_Board();
    cout << "--------------------------------------\n";

    // -------- ask if plyaer wants to play first --------
    cout << "Player 1 always plays first.\n";
    string player_input;
    while (true)
    {
        cout << "Who do you want to be? [1/2] or [q] to quit: ";
        cin >> player_input;
        if (cin.fail() || (player_input != "1" && player_input != "2" && player_input != "q"))
        {
            cin.clear();                                         // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            cout << "Invalid input, please try again.\n";
            continue;
        }
        break;
    }
    if (player_input == "1")
    {
        printf("You are player 1\n");   
    }
    else if (player_input == "2")
    {
        printf("You are player 2\n");
    }
    else 
    {
        cout << "Exiting...\n";
        return save_and_exit(mcts_tree);
    }

    // --------- start the game ---------    
    clear_screen();
    MCTS_leaf* current_node = mcts_tree;
    while (true)
    {
        // populate the possible moves of the current node
        current_node->state.list_all_possible_moves(current_node->state.get_current_player());
        // if current node is terminal, end the game
        if (current_node->state.TerminalState() != -1)
        {
            cout << "Game over!\n";
            cout << "Player " << current_node->state.TerminalState() << " won!\n";
            return save_and_exit(mcts_tree);
        }
        // if the current player is the user
        if (current_node->state.get_current_player() == stoi(player_input))
        {
            cout << "Your turn!\n";
            current_node->state.list_all_possible_moves(current_node->state.get_current_player());
            // select all source squares
            for (auto move : current_node->state.possible_moves)
            {
                current_node->state.get_board()->get_Piece(move.get_src_y(), move.get_src_x())->select(-1,-1);
                current_node->state.get_board()->get_Piece(move.get_dest_y(), move.get_dest_x())->select(-1,-1);
                int enemy_y = move.get_enemy_y();
                int enemy_x = move.get_enemy_x();
                if (enemy_x != -1 || enemy_y != -1) current_node->state.get_board()->get_Piece(enemy_y, enemy_x)->select(enemy_y, enemy_x);
            }
            current_node->state.get_board()->print_Board();
            current_node->state.print_all_moves();
            // deselect all squares
            for (auto move : current_node->state.possible_moves)
            {
                current_node->state.get_board()->get_Piece(move.get_src_y(), move.get_src_x())->deselect();
                current_node->state.get_board()->get_Piece(move.get_dest_y(), move.get_dest_x())->deselect();
                int enemy_y = move.get_enemy_y();
                int enemy_x = move.get_enemy_x();
                if (enemy_x != -1 || enemy_y != -1) current_node->state.get_board()->get_Piece(enemy_y, enemy_x)->deselect();            
            }
            // get user input
            string usr_choice_in = "";
            int usr_choice = 0;
            while (true)
            {
                cout << "Select a move or [q] to quit: ";
                cin >> usr_choice_in;
                if (usr_choice_in == "q") break;
                usr_choice = stoi(usr_choice_in);
                if ((cin.fail() || usr_choice < 1 || usr_choice > current_node->state.possible_moves.size()))
                {
                    cin.clear();                                         // clear the error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
                    cout << "Invalid input, please try again.\n";
                    continue;
                }
                break;
            }
            // optional quit option
            if (usr_choice_in == "q")
            {
                cout << "Exiting...\n";
                return save_and_exit(mcts_tree);
            }
            // perform the move by searching the children of the current node and finding the one that matches the move
            string selected_move_str = current_node->state.possible_moves.at(usr_choice - 1).get_move_info();
            // select a move that has not been explored yet
            // load all of the children into a set
            map<string, MCTS_leaf *> moves_children;
            for (MCTS_leaf *child : current_node->children)
            {
                moves_children.insert({child->get_move_info(), child});
            }
            // check if this move has already been explored by the AI
            bool found = false;
            for (Move move : current_node->state.possible_moves)
            {
                // map.find() returns an iterator to the element if found, or end() if not found
                auto it = moves_children.find(selected_move_str);
                if (it != moves_children.end())
                {
                    // if the child move matches, we can select it
                    found = true;
                    current_node = it->second;
                    break;
                }
            }
            if (!found)
            {
                Move selected_move = current_node->state.possible_moves.at(usr_choice - 1);
                // if the AI has not explored this move yet, we need to create a new child node
                // create a new game state with the selected move
                GameState new_game_state = current_node->state.clone();
                // change the player of the new game state
                new_game_state.switch_player();
                // perform move
                Board* tmp_board = new_game_state.get_board();
                selected_move.perform_move(tmp_board, selected_move);
                // populate the possible moves of the new game state
                new_game_state.list_all_possible_moves(new_game_state.get_current_player());
                // create a new child node with the new game state and add to the tree
                MCTS_leaf* new_child = new MCTS_leaf(new_game_state, selected_move, current_node);
                current_node->children.push_back(new_child);
                current_node = new_child;

                // train the AI on this new node
                train(current_node, 20);
            }
            printf("board after your move:\n");
            current_node->state.get_board()->print_Board();
            cout << "--------------------------------------\n";
        }
        // if the current player is the AI
        else
        {
            cout << "AI's turn!\n";
            // AI will play
            // select the best move from the MCTS tree
            MCTS_leaf* newnode = select_most_visited_child(current_node);
            cout << "AI selected move: ";
            newnode->print_move();
            // if newnode is current_node
            if (newnode == current_node)
            {
                // we are not at the terminal state,
                // which means the AI has not expolred this part of the tree yet.
                // so we need to expand the tree by training the ai
                train(newnode, 30);
                // select the best child of the new node
                newnode = select_most_visited_child(newnode);
            }

            current_node = newnode;
            printf("board after AI's move:\n");
            current_node->state.get_board()->print_Board();
            cout << "--------------------------------------\n";
        }
    }
    
    return save_and_exit(mcts_tree);
}

// ----- TRAINING MODE ------ //
int choice2()
{
    // load the tree from file and reconstruct tree
    ifstream input_file("mcts_tree.txt");
    MCTS_leaf *mcts_tree;
    if (input_file.is_open())
    {
        cout << "loading tree from file...\n";
        string raw_input;
        getline(input_file, raw_input);
        mcts_tree = load_tree(raw_input);
    }
    else
    {
        cout << "no tree found, creating new tree...\n";
        // create new tree node
        Board board = create_board("default");
        GameState game_state = GameState(board, PLAYER1);
        mcts_tree = new MCTS_leaf(game_state, Move(-1, -1, -1, -1, false, -1, -1), nullptr, {}, 0, 0, true, false);
    }
    // how many iterations to run
    int num_iterations = 0;
    while (true)
    {
        cout << "How many iterations do you want to run? ";
        cin >> num_iterations;
        if (cin.fail() || num_iterations < 1)
        {
            cin.clear();                                         // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            cout << "Invalid input, please try again.\n";
            continue;
        }
        break;
    }

    // run mcts algorithm
    DEBUG_PRINT("-------------------------------------- STARTING TRAINING --------------------------------------\n");
    auto start = chrono::high_resolution_clock::now();
    train(mcts_tree, num_iterations);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    DEBUG_PRINT("-------------------------------------- TRAINING DONE --------------------------------------\n");
    cout << "simulated " << mcts_tree->total_games << " games" << " in " << duration.count() << " ms" << endl;
    // save tree to file and destroy tree
    cout << "saving tree to file...\n";
    return save_and_exit(mcts_tree);
}