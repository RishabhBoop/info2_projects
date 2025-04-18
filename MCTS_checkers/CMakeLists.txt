cmake_minimum_required(VERSION 3.10.0)
project(MCTS_checkers VERSION 0.1.0 LANGUAGES C CXX)

add_executable(MCTS_checkers main.cpp)
add_library(state state.cpp state.hpp)

target_link_libraries(MCTS_checkers PUBLIC state)