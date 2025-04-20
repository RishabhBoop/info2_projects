cmake_minimum_required(VERSION 3.10.0)
project(MCTS_checkers VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# --- Libraries ---
# Define libraries used by both main executable and tests
add_library(CLASSES classes.cpp classes.hpp)
add_library(MCTS_LOGIC mcts_algorithm.cpp mcts_algorithm.hpp)

# --- Main Executable ---
# Define a single executable target
add_executable(checkers_exec main.cpp)

# Link main executable against libraries

target_link_libraries(checkers_exec PUBLIC MCTS_LOGIC CLASSES)

# --- Configuration-Specific Settings ---
# Add DEBUG definition for Debug builds to all relevant targets
target_compile_definitions(CLASSES PUBLIC $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(MCTS_LOGIC PUBLIC $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(checkers_exec PUBLIC $<$<CONFIG:Debug>:DEBUG>)

# --- Testing Setup ---
include(CTest)

# Only define test-specific targets and settings if BUILD_TESTING is ON
# (VS Code CMake Tools usually sets BUILD_TESTING=ON when you interact with the Test Explorer)
if(BUILD_TESTING)
    enable_testing()

    # Add executable specifically for testing
    add_executable(checkers_exec_test main.cpp)
    add_library(TESTS tests.cpp tests.hpp)

    # Add TESTING definition for all targets *when building for tests*
    # Note: This applies TESTING definition in addition to DEBUG if the build type is Debug AND BUILD_TESTING is ON
    target_compile_definitions(checkers_exec_test PUBLIC TESTING)
    # Also add to TESTS library itself if it needs the definition
    target_compile_definitions(TESTS PUBLIC TESTING)


    # Link the test executable
    target_link_libraries(checkers_exec_test PUBLIC TESTS CLASSES MCTS_LOGIC)

    # Register the test with CTest
    add_test(NAME checkers_core_test COMMAND checkers_exec_test) # Changed NAME for clarity
endif()