#include "tests.hpp"
#include "helper_funcs.hpp"

int all_tests()
{
    printf("Testing Loading and Saving a tree + Training...\n");
    int testres = test_load_save();
    if (testres != 0)
        return testres;
    printf("Saving and loading tree test passed!\n");
    printf("------\n");
    printf("Testing jumping moves...\n");
    testres = test_jump();
    if (testres != 0)
        return testres;
    printf("Jumping test passed!\n");
    printf("------\n");
    printf("Testing if king is set correctly...\n");
    testres = test_king();
    if (testres != 0)
        return testres;
    printf("King test passed!\n");
    return testres;
}

int test_load_save()
{
    // create, expand and save tree
    array<array<Piece, 8>, 8> start_board = create_board("default");
    GameState init(Board(start_board), 1);
    Move default_move(-1, -1, -1, -1, false, -1, -1);
    MCTS_leaf *tree1 = new MCTS_leaf(init, default_move, nullptr, {}, 0, 0, false, false);
#ifdef DEBUG
    printf("\tcreated tree\n");
#endif
    train(tree1, 5);
#ifdef DEBUG
    printf("\ttrained tree\n");
#endif
    // save tree to file
    ofstream output_file("mcts_tree.txt");
    save_tree(tree1, output_file);
    output_file.close();
#ifdef DEBUG
    printf("\tsaved tree\n");
#endif
    // load the tree from file and reconstruct tree
    ifstream input_file("mcts_tree.txt");
    string raw_input;
    getline(input_file, raw_input);
#ifdef DEBUG
    printf("\tread tree file\n");
#endif
    MCTS_leaf *tree2 = load_tree(raw_input);
    try
    {
        compare_trees(tree1, tree2);
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
#ifdef DEBUG
    printf("\treconstructed tree successfully\n");
#endif
    destroy_tree(tree1);
    destroy_tree(tree2);
#ifdef DEBUG
    printf("\tdestroyed trees \n==> Graceful exit\n");
    printf("---------------------------------\n");
#endif

    return 0;
}

int test_jump()
{
    // create, expand and save tree
    array<array<Piece, 8>, 8> start_board = create_board("jump-test");
    GameState init(Board(start_board), 1);
    Board *tmp_board = init.get_board();
#ifdef DEBUG
    // ---- for printing the board ----
    cout << "Before moving:\n";
    tmp_board->print_Board();
// ---------------------------------
#endif
    Move default_move(-1, -1, -1, -1, false, -1, -1);
    MCTS_leaf *tree1 = new MCTS_leaf(init, default_move, nullptr, {}, 0, 0, false, false);
    // clone the state, perform a move and check if the piece is a king
    GameState tmp_state = tree1->state.clone();
    // switch the player
    tmp_state.switch_player();
    // get possible moves
    tmp_state.list_all_possible_moves(tmp_state.get_current_player());
#ifdef DEBUG
    cout << "\tPossible moves:\n\t\t";
    tmp_state.print_all_moves();
#endif
    // perform the move
    tmp_board = tmp_state.get_board();
    tmp_state.possible_moves[0].perform_move(tmp_board, tmp_state.possible_moves[0]);
#ifdef DEBUG
    // -- for printing the board ----
    cout << "\tAfter performing the move:\n";
    tmp_board->print_Board();
// ------------------------------
#endif
    Piece moved_piece = tmp_board->get_Piece(tmp_state.possible_moves[0].get_dest_y(), tmp_state.possible_moves[0].get_dest_x());
    // check if jumped piece was removed
    Piece jumped_piece = tmp_board->get_Piece(tmp_state.possible_moves[0].get_enemy_y(), tmp_state.possible_moves[0].get_enemy_x());
    if (!jumped_piece.is_empty())
    {
        printf("\tJumped piece was not removed!\n");
        return 1;
    }
    destroy_tree(tree1);
#ifdef DEBUG
    printf("\tdestroyed trees \n==> Graceful exit\n");
    printf("---------------------------------\n");
#endif
    return 0;
}

int test_king()
{
    // create, expand and save tree
    array<array<Piece, 8>, 8> start_board = create_board("king-test");
    GameState init(Board(start_board), 1);
    Board *tmp_board = init.get_board();
#ifdef DEBUG
    // ---- for printing the board ----
    cout << "Before moving:\n";
    tmp_board->print_Board();
// ---------------------------------
#endif
    Move default_move(-1, -1, -1, -1, false, -1, -1);
    MCTS_leaf *tree1 = new MCTS_leaf(init, default_move, nullptr, {}, 0, 0, false, false);
#ifdef DEBUG
    printf("\tcreated tree\n");
#endif
    // clone the state, perform a move and check if the piece is a king
    GameState tmp_state = tree1->state.clone();
    // switch the player
    tmp_state.switch_player();
    // get possible moves
    tmp_state.list_all_possible_moves(tmp_state.get_current_player());
#ifdef DEBUG
    cout << "\tPossible moves:\n\t\t";
    tmp_state.print_all_moves();
#endif
    // perform the move
    tmp_board = tmp_state.get_board();
    tmp_state.possible_moves[0].perform_move(tmp_board, tmp_state.possible_moves[0]);
#ifdef DEBUG
    // -- for printing the board ----
    cout << "\tAfter performing the move:\n";
    tmp_board->print_Board();
// ------------------------------
#endif
    Piece moved_piece = tmp_board->get_Piece(tmp_state.possible_moves[0].get_dest_y(), tmp_state.possible_moves[0].get_dest_x());
    if (!moved_piece.get_king())
    {
        printf("\tPiece is not a king!\n");
        return 1;
    }
    destroy_tree(tree1);
#ifdef DEBUG
    printf("\tdestroyed trees \n==> Graceful exit\n");
    printf("---------------------------------\n");
#endif
    return 0;
}

void compare_trees(MCTS_leaf *tree1, MCTS_leaf *tree2)
{
    // if both are nullptr then return
    if (tree1 == nullptr && tree2 == nullptr)
    {
        return;
    }
    if ((tree1 == nullptr) != (tree2 == nullptr))
    {
        throw runtime_error("nullptr mismatch\n");
        return;
    }
    if (tree1->state.get_state_info() != tree2->state.get_state_info())
    {
        throw runtime_error("GameState mismatch\n");
        return;
    }
    if (tree1->get_move_info() != tree2->get_move_info())
    {
        throw runtime_error("Move mismatch\n");
        return;
    }
    if (tree1->children.size() != tree2->children.size())
        throw runtime_error("Tree size mismatch\n");

    for (size_t i = 0; i < tree1->children.size(); i++)
    {
        // if (i >= tree2->children.size())
        // {
        //     throw runtime_error("Number of children mismatch\n");
        //     return;
        // }
        compare_trees(tree1->children[i], tree2->children[i]);
    }
}