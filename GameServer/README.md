# Project 2 - Game Server
> THIS ONLY WORKS ON LINUX-BASED SYSTEMS and on MAC OS!
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
cd info2_projects/GameServer # change to the project directory
mkdir build # create a build directory; optional
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
This project creates a simple game server that allows you to play games against other players or against an AI.
It consists of a server that listens for incoming connections and a client that connects to the server.
The server is implemented in C++ and uses the sys/socket library to handle network communication.

## Protocol
### 1. Client connects to server
Client sends:
```
TEXT:<Hello>\a
```
Server echoes back the message:
```
TEXT:<Hello>\a
```
Client then checks if the sent and received message is the same:
If yes: 
```
PERFECT:<>\a
```
If no:
[TODO]

### 2. Server sends welcome message
```
TEXT:<PROJECT 2>\a      // Server sends welcome message
```

### 3. Server asks for the player's name
Server sends:
```
QUESTION_STR:<What is your name?>\a
```
Client responds with:
```
ANSWER:<Your-Name>\a
```

### 4. Choose a game
Server sends a question:
```
QUESTION:<What game do you want to play?;checkers>\a
```
Client answers with:
```
ANSWER:<Game-Name>\a
```
### 5. Ask if the player wants to play against an AI or a person
Server sends:
```
QUESTION:<Do you want to play against an AI or a person?;Person,AI>\a
```
Client responds with:
```
ANSWER:<0/1 (Person/AI)>\a
```
#### 5.1 If the player chooses to play against a person and there are no people available 
Server sends:
```
WAITING:<Waiting for another player to join>\a
```
The TEXT_LOBBY message turns on a flag in the client. The client will then loop an animation until the server sends a message that the game has started. The client will check for:
```
WAITING_END:<>\a
```

### 6. Game starts
Server sends:
```
TEXT:<Game started>\a
```

#### 6.1 Checkers Game Logic
The server sends the board:
```
CHEKCERS_STATE:<state stringified>\a
```
The client sends the chosen move:
```
CHECKERS_MOVES:<Your turn>\a
```
or if the user wants to quit:
```
CHECKERS_MOVES:<q>\a
```

#### 6.2 Checkers Game End
The server sends the game end message:
```
CHECKERS_END:<msg>\a
```
And a goodbye message:
```
GOODBYE:<>\a
```

## 7. End of the game
Server sends:
```
GOODBYE:<>\a
```
Client closes the socket.