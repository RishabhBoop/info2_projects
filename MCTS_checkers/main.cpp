#include "classes.hpp"
#include "mcts_algorithm.hpp"
#ifdef TESTING
#include "tests.hpp"
#endif
#include <chrono>

using namespace std;

int main()
{

#ifdef DEBUG
    printf("Debug mode\n");
#endif

#ifdef TESTING
    printf("Testing mode\n");
    return all_tests();
#endif

    printf("Welcome to checkers\n");
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

    // run mcts algorithm
#ifdef DEBUG
    printf("-------------------------------------- STARTING TRAINING --------------------------------------\n");
#endif
    auto start = chrono::high_resolution_clock::now();
    train(mcts_tree, 1000000);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
#ifdef DEBUG
    printf("-------------------------------------- TRAINING DONE --------------------------------------\n");
#endif
    cout << "simulated " << mcts_tree->total_games << " games!" << endl;
    cout << "training took " << duration.count() << " ms" << endl;
    // save tree to file
    ofstream output_file("mcts_tree.txt");
    if (output_file.is_open())
    {
        save_tree(mcts_tree, output_file);
        output_file.close();
    }
    else
    {
        cout << "Unable to open file\n";
    }
#ifdef DEBUG
    printf("saved tree to file!\n");
#endif
    // destroy the tree
    destroy_tree(mcts_tree);
#ifdef DEBUG
    printf("Tree destroyed!\n");
#endif

    printf("goodbye!\n");
    return 0;
}
