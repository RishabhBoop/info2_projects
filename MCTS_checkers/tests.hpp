#ifndef TESTS_HPP
#define TESTS_HPP

#include "classes.hpp"
#include "mcts_algorithm.hpp"

using namespace std;

int all_tests();

int test_load_save();

int test_jump();

int test_king();

void compare_trees(MCTS_leaf *, MCTS_leaf *);

#endif