# Project 1 - Monte Carlo Tree Search Algorithm
## What is this project?
This project implements a Monte Carlo Tree Search (MCTS) algorithm to play the game of checkers. It is the first project of Informatik 2 in SS25 at HKA.
## Building the project
You must have at least `cmake 3.22.1` installed.
```bash
git clone https://github.com/RishabhBoop/info2_projects.git
cd info2_projects/MCTS_checkers
mkdir build
cmake -S . -B build
cmake --build build
```
The executable will be created in the `/bin` directory.
### Building with different configurations
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

