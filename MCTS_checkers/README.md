# Project 1 - Monte Carlo Tree Search Algorithm
For the german version of this readme, please refer to the [README_DE.md](README_DE.md) file.
## Table of contents
1. [Building the project](#building-the-project)
2. [Project description](#what-is-this-project)
## Building the project
### Requirements
You must have at least `cmake 3.22.1` installed and an Address Sanitizer compatible compiler.
Because the project is using cmake, you can build it on any platform that supports cmake. 
As of now, this project has been tested only been tested on Linux-based systems (Ubuntu 24.04 and 24.10). <br>
The steps below are for building on linux-based systems. If you are not using a linux-based system, you must adapt the steps accordingly.
### Building on Linux
```bash
git clone https://github.com/RishabhBoop/info2_projects.git # clone the repo
cd info2_projects/MCTS_checkers # change to the project directory
mkdir build # create a build directory
cmake -S . -B build # run cmake to generate the build files
cmake --build build # build the project
```
The executable will be created in the `/bin` directory.
```bash
# ----- for release build -----
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
./build/checkers_exec
# ----- for debug build -----
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
./build/checkers_exec
# ----- for testing -----
cd build && ctest -C build --output-on-failure
```

## What is this project?
This project implements a Monte Carlo Tree Search (MCTS) algorithm to play the game of checkers. It is the first project of Informatik 2 in SS25 at HKA with Prof. Hanuschkin.
## MCTS Algorithm
When we want the computer to play role-based games with a definitive end state (e.g. TicTacToe, checkers, chess, go), we could naivley store every possible state of the game in a tree and then traverse the tree to find the best move. However, this is not feasible for games with a large number of possible states (e.g. chess, go). Therefore, we use the Monte Carlo Tree Search (MCTS) algorithm to find the best move. <br>
The MCTS algorithm works by only creating and exploring a small number of nodes in the tree. It does this by using a combination of four steps: