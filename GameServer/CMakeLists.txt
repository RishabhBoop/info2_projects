cmake_minimum_required(VERSION 3.22.1)
project(GameServer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)



# --- Libraries ---
add_library(CLASSES ./server/classes.cpp ./server/classes.hpp)
add_library(MCTS_LOGIC ./server/mcts_algorithm.cpp ./server/mcts_algorithm.hpp)
add_library(REQEST_HELPERS request_helpers.cpp request_helpers.hpp includes.hpp)

# --- Add Debug Definition ---
target_compile_definitions(REQEST_HELPERS PUBLIC $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(CLASSES PUBLIC $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(MCTS_LOGIC PUBLIC $<$<CONFIG:Debug>:DEBUG>)
target_link_libraries(REQEST_HELPERS PUBLIC MCTS_LOGIC CLASSES)

# --- Subdirectories ---
add_subdirectory(server)
add_subdirectory(client)
