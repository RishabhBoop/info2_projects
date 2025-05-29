cmake_minimum_required(VERSION 3.22.1)
project(MCTS_checkers VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
# If Debug, then add cxx flag
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug build detected")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message(STATUS "Detected Clang or GCC compiler")
        add_compile_options(-fsanitize=address -g)
        add_link_options(-fsanitize=address -static-libasan -g)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        message(STATUS "Detected AppleClang compiler")
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(STATUS "Detected MSVC compiler")
        add_compile_options(/fsanitize=address)
        add_link_options(/fsanitize=address)
    else()
        message(SEND_ERROR "AddressSanitizer is not supported on this compiler.")
    endif()
endif()

# --- Libraries ---
# Define libraries used by both main executable and tests
add_library(CLIENT_REQEST_HELPERS request_helpers.cpp request_helpers.hpp)

# --- Main Executable ---
# Define a single executable target
add_executable(client main.cpp)

# Link main executable against libraries
target_link_libraries(client PUBLIC CLIENT_REQEST_HELPERS)