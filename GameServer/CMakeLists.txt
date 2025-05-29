cmake_minimum_required(VERSION 3.22.1)
project(GameServer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# --- Libraries ---
add_library(REQEST_HELPERS request_helpers.cpp request_helpers.hpp)
# target_include_directories(REQEST_HELPERS
#   PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR} # This is the directory containing request_helpers.hpp
# )
# --- Subdirectories ---
add_subdirectory(server)
add_subdirectory(client)
