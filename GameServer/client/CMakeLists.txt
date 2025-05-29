cmake_minimum_required(VERSION 3.22.1)
project(MCTS_checkers VERSION 0.1.0 LANGUAGES C CXX)


# --- Main Executable ---
# Define a single executable target
add_executable(client main.cpp)

# Link main executable against libraries
target_link_libraries(client PUBLIC REQEST_HELPERS)