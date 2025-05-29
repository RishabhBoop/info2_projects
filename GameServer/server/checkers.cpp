#include "classes.hpp"
#include "mcts_algorithm.hpp"
#include <chrono>
#include <limits>

using namespace std;

int save_and_exit(MCTS_leaf *mcts_tree)
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

int train_ai()
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
    int num_iterations = 10000;

    // run mcts algorithm
    auto start = chrono::high_resolution_clock::now();
    train(mcts_tree, num_iterations);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "simulated " << mcts_tree->total_games << " games" << " in " << duration.count() << " ms" << endl;
    cout << "Wins: " << mcts_tree->wins << endl;
    // save tree to file and destroy tree
    cout << "saving tree to file...\n";
    return save_and_exit(mcts_tree);
}

int play_v_ai()
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
            int res = train_ai();
            if (res != 0)
                return res;
            cout << "let us try again...\n";
            continue;
        }
        break;
    }

    // we now have a tree, so we can play the game


    return 0;
}