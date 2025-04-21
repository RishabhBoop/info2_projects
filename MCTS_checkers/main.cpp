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

#ifdef DEBUG
#define DEBUG_PRINT(X) cout << X;
#else
#define DEBUG_PRINT(X)
#endif

using namespace std;

void print_prj_banner();
int choice1();
int choice2();

int main()
{
    clear_screen();
    print_prj_banner();
    DEBUG_PRINT("Debug mode activated\n");
    TEST();
    // -------- First user input: programm mode --------
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
        if (cin.fail() || choice > 3 || choice < 1)
        {
            cin.clear();                                         // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            cout << "Invalid input, please try again.\n";
            continue;
        }
        break;
    }
    if (choice == 1)
    {
        return choice1();
    }
    else if (choice == 2)
    {
        return choice2();
    }
    else
    {
        cout << "exiting...\n";
        return 0;
    }

    return 0;
}

void print_prj_banner()
{
    cout << BOLDGREEN <<
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
    int player_input;
    while (true)
    {
        cout << "Who do you want to be? (1/2)";
        cin >> player_input;
        if (cin.fail() || (player_input != 1 && player_input != 2 && player_input != 'q'))
        {
            cin.clear();                                         // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            cout << "Invalid input, please try again.\n";
            continue;
        }
        break;
    }
    if (player_input == 1)
    {
        printf("You are player 1\n");   
    }
    else if (player_input == 2)
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
        // if current node is terminal, end the game
        if (current_node->state.TerminalState() != -1)
        {
            cout << "Game over!\n";
            // if (current_node->state.TerminalState() == player_input)
            // {
            //     cout << "You lost!\n";
            // }
            // else
            // {
            //     cout << "You won!\n";
            // }
            cout << "Player " << current_node->state.TerminalState() << " won!\n";
            return save_and_exit(mcts_tree);
        }
        // if the current player is the user
        if (current_node->state.get_current_player() == player_input)
        {
            cout << "Your turn!\n";
            current_node->state.list_all_possible_moves(current_node->state.get_current_player());
            // select all source squares
            for (auto move : current_node->state.possible_moves)
            {
                current_node->state.get_board()->get_Piece(move.get_src_y(), move.get_src_x())->select(-1,-1);
                current_node->state.get_board()->get_Piece(move.get_dest_y(), move.get_dest_x())->select(-1,-1);
                int enemy_x = move.get_enemy_x();
                int enemy_y = move.get_enemy_y();
                current_node->state.get_board()->get_Piece(enemy_y, enemy_x)->select(enemy_y,enemy_x);
            }
            current_node->state.get_board()->print_Board();
            current_node->state.print_all_moves();
            // deselect all squares
            for (auto move : current_node->state.possible_moves)
            {
                current_node->state.get_board()->get_Piece(move.get_src_y(), move.get_src_x())->deselect();
                current_node->state.get_board()->get_Piece(move.get_dest_y(), move.get_dest_x())->deselect();
                current_node->state.get_board()->get_Piece(move.get_enemy_y(), move.get_enemy_x())->deselect();
            }
            // get user input
            int usr_choice = 0;
            while (true)
            {
                cout << "Select a move: ";
                cin >> usr_choice;
                if ((cin.fail() || usr_choice < 1 || usr_choice > current_node->state.possible_moves.size()) && usr_choice != 'q')
                {
                    cin.clear();                                         // clear the error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
                    cout << "Invalid input, please try again.\n";
                    continue;
                }
                break;
            }
            // optional quit option
            if (usr_choice == 'q')
            {
                cout << "Exiting...\n";
                return save_and_exit(mcts_tree);
            }
            // perform the move by searching the children of the current node and finding the one that matches the move
            string selected_move = current_node->state.possible_moves.at(usr_choice - 1).get_move_info();
            for (MCTS_leaf *child : current_node->children)
            {
                // if the child move matches the selected move, set the current node to the child
                if (child->get_move_info() == selected_move)
                {
                    current_node = child;
                    break;
                }
                else
                {
                    // if the child move does not match,
                    // it means that the ai has not explored this part of the tree yet.
                    // so we need to train the ai
                    train(current_node, 1000);
                    // select the best child of the new node
                    current_node = select_best_child(current_node);
                }
            }
        }
        // if the current player is the AI
        else
        {
            cout << "AI's turn!\n";
            // AI will play
            // select the best move from the MCTS tree
            MCTS_leaf* newnode = select_best_child(current_node);
            // if newnode is current_node
            if (newnode == current_node)
            {
                // we are not at the terminal state,
                // which means the AI has not expolred this part of the tree yet.
                // so we need to expand the tree by training the ai
                train(newnode, 1000);
                // select the best child of the new node
                newnode = select_best_child(newnode);
            }
            current_node = newnode;
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