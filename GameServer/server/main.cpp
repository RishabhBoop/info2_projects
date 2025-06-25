#include "../request_helpers.hpp"

using namespace std;

void thread_function(Player *);
int gameplay_ai(Player *);
int gameplay_multiplayer(Session *);

vector<Player *> ALL_PLAYERS;   // global variable to keep track of the number of connections
vector<Session *> ALL_SESSIONS; // global variable to keep track of all sessions
vector<thread> ALL_THREADS;     // global variable to keep track of all threads
mutex all_players_mutex;
mutex all_sessions_mutex;

int main()
{
    print_banner();
    int sock = init_server_sock(); // initialize the server socket
    if (sock < 0)
    {
        cerr << ERROR << "Error initializing server socket" << RESET << endl;
        return -1; // exit if socket initialization failed
    }

    fd_set read_fds;   // file descriptor set for select
    struct timeval tv; // Struct to specify timeout

    while (true)
    {
        FD_ZERO(&read_fds);      // clear the set
        FD_SET(sock, &read_fds); // add the server socket to the set

        tv.tv_sec = 30; // set timeout to 30s
        tv.tv_usec = 0; // no microseconds

        int activity = select(sock + 1, &read_fds, nullptr, nullptr, &tv);
        if (activity == 0)
        {
            // Timeout occurred, no activity on the socket
            DEBUG_PRINT("No activity on the socket for 5 seconds, breaking...\n");
            cout << INFO << "Timeout occured, no more taking connections" << RESET << endl;
            break; // exit the loop if no activity for 5 seconds
        }
        else if (activity < 0)
        {
            cerr << ERROR << "Error in select: " << strerror(errno) << RESET << endl;
            break;
        }

        /* Accept connection
         * If a client tries to connect, the server will accept the connection and create a new socket for communication with that client.
         */
        int client_sock = accept(sock, nullptr, nullptr);
        if (client_sock < 0)
        {
            cerr << ERROR << "Error accepting connection: " << strerror(errno) << RESET << endl;
            close(sock);
            return -1;
        }
        Player *player = new Player();   // create a new player object to handle the client connection
        player->set_socket(client_sock); // set the socket for the player object

        {
            lock_guard<mutex> lock(all_players_mutex);
            ALL_PLAYERS.push_back(player); // add the player object to the global list of players
        }
        ALL_THREADS.push_back(thread(thread_function, player)); // create a new thread to handle the client configuration
        // a thread will be created to handle the gameplay
        DEBUG_PRINT("Accepted connection from client\n");
    }

#pragma region Cleanup
    // join all threads
    for (int i = 0; i < ALL_THREADS.size(); i++)
    {
        if (ALL_THREADS[i].joinable())
        {
            ALL_THREADS[i].join(); // wait for the thread to finish
        }
    }
    DEBUG_PRINT("All threads joined successfully\n");

    // delete the player object after the thread is done
    {
        lock_guard<mutex> lock(all_players_mutex);
        for (auto p : ALL_PLAYERS)
        {
            if (p != nullptr)
                delete p; // clean up player objects
        }
    }

    // close sockets
    close(sock);
    cout << INFO << "Server socket closed" << RESET << endl;
#pragma endregion
    return 0;
}

#pragma region Thread for a Client
void thread_function(Player *player)
{
    int client_sock = player->get_socket();
    DEBUG_PRINT("Thread started for client socket: " + to_string(client_sock) + "\n");

#pragma region Handshake
    /* ---------------- Handshake -------------------------- */
    DEBUG_PRINT("------------------ Handshake with client -----------------\n");
    string response = receive_from(client_sock); // receive data from the client
    DEBUG_PRINT("Received from client: " + response + "\n");
    // echo response back to client (unblock client recv)
    DEBUG_PRINT("Echoing response back to client...\n");
    if (send_to(client_sock, response) < 0)
    {
        cerr << ERROR << "Error sending response back to client: " << strerror(errno) << RESET << endl;
    }
    DEBUG_PRINT("Response sent back to client\n");
    response = receive_from(client_sock); // receive data from the client again (should be "PERFECT")
    if (parsed_response(response)["type"].front() == "PERFECT")
    {
        // DEBUG_PRINT("Client confirmed handshake successfully\n");
        cout << SUCCESS << "Client confirmed handshake successfully" << RESET << endl;
    }
    else
    {
        cerr << ERROR << "Client did not confirm handshake successfully: " << response << RESET << endl;
    }
/* ---------------- End Handshake -------------------------- */
#pragma endregion

#pragma region Ask for Nickname
    /* ---------------- ask for nickname --------------------------*/
    DEBUG_PRINT("------------------ Asking client for nickname -----------------\n");
    string question = "What is your nickname?";
    if (send_to(client_sock, "QUESTION_STR", question) < 0)
    {
        cerr << ERROR << "Error sending question to client: " << strerror(errno) << RESET << endl;
        close(client_sock);
        return;
    }
    // receive the answer from the client
    response = receive_from(client_sock); // receive data from the client again (should be "ANSWER")
    if (parsed_response(response)["type"].front() == "ANSWER")
    {
        // DEBUG_PRINT("Client answered the question successfully\n");
        DEBUG_PRINT("Client answered the question successfully\n");
        // process the answer
        string nickname;
        string *ptr_nickname = &nickname;
        process_response(parsed_response(response), ptr_nickname);
        // add the nickname to the player object
        player->set_nickname(nickname);
        // print the nickname
        DEBUG_PRINT("Client's nickname: " + nickname + "\n");
        // cout << INFO << "Client's nickname: " << nickname << RESET << endl;
    }
    else
    {
        cerr << ERROR << "Client did not answer the question successfully: " << response << RESET << endl;
    }
/* ---------------- End ask for nickname --------------------------*/
#pragma endregion

#pragma region Ask for Game Choice
    /* ---------------- Send Question which game --------------------------*/
    DEBUG_PRINT("------------------ Asking client which game to play -----------------\n");
    vector<string> options = {"Checkers"};
    question = question_to_string("Which game do you want to play?", options);
    if (send_to(client_sock, "QUESTION", question) < 0)
    {
        cerr << ERROR << "Error sending question to client: " << strerror(errno) << RESET << endl;
        close(client_sock);
        return;
    }
    // receive the answer from the client
    response = receive_from(client_sock); // receive data from the client again (should be "ANSWER")
    int chosen_answer = -1;
    if (parsed_response(response)["type"].front() == "ANSWER")
    {
        // DEBUG_PRINT("Client answered the question successfully\n");
        cout << SUCCESS << "Client answered the question successfully" << RESET << endl;
        // process the answer
        int *ptr_chosen_answer = &chosen_answer;
        process_response(parsed_response(response), ptr_chosen_answer);
        // add the chosen game to the player object
        player->set_chosen_game(chosen_answer);
        // print the chosen game
        DEBUG_PRINT("Client chose option: " + to_string(chosen_answer) + "\n");
    }
    else
    {
        cerr << ERROR << "Client did not answer the question successfully: " << response << RESET << endl;
    }
/* ---------------- End Send Question which game --------------------------*/
#pragma endregion

#pragma region Ask for Game Mode
    /* ---------------- Send question single-player or multiplayer --------------------------*/
    DEBUG_PRINT("------------------ Asking client if they want to play against player or against AI -----------------\n");
    question = "Do you want to play against another player or against AI?";
    options = {"Player", "AI"}; // options for the question
    if (send_to(client_sock, "QUESTION", question_to_string(question, options)) < 0)
    {
        cerr << ERROR << "Error sending question to client: " << strerror(errno) << RESET << endl;
        close(client_sock);
        return;
    }
    // receive the answer from the client
    response = receive_from(client_sock); // receive data from the client again (should be "ANSWER")
    int chosen_game_mode = -1;            // -1 means no answer received
    if (parsed_response(response)["type"].front() == "ANSWER")
    {
        // DEBUG_PRINT("Client answered the question successfully\n");
        cout << SUCCESS << "Client answered the question successfully" << RESET << endl;
        // process the answer
        int *ptr_chosen_game_mode = &chosen_game_mode;
        process_response(parsed_response(response), ptr_chosen_game_mode);
        // add the chosen game mode to the player object
        player->set_chosen_game_mode(chosen_game_mode);
        // print the chosen game mode
        DEBUG_PRINT("Client chose game mode: " + to_string(chosen_game_mode) + "\n");
    }
    else
    {
        cerr << ERROR << "Client did not answer the question successfully: " << response << RESET << endl;
    }
/* ---------------- End Send question single-player or multiplayer --------------------------*/
#pragma endregion

#pragma region Put Player into Session

    if (player->get_chosen_game_mode() == 1)
    {
        // 1 means player chose to play against AI
        {
            lock_guard<mutex> lock(all_sessions_mutex);
            Session *new_session = new Session(player, nullptr); // create a new session with the player as player1
            ALL_SESSIONS.push_back(new_session);                 // add the new session to the global list of sessions
            player->set_session(new_session);                    // set the session for the player object
        }
        DEBUG_PRINT("New session created for player\n");
    }
    else if (player->get_chosen_game_mode() == 0)
    {
        // 0 means player chose to play against another player
        bool inside_session = false; // flag to check if the player is added to an existing session
        {
            lock_guard<mutex> lock(all_sessions_mutex);
            for (auto session : ALL_SESSIONS)
            {
                // check if the session is full
                // if not, check if the player1 wants to play mutliplayer
                if (session->player2 == nullptr && session->player1->get_chosen_game_mode() == 0)
                {
                    // if player2 is not set, set the player as player2
                    session->player2 = player;
                    player->set_session(session); // set the session for the player object
                    inside_session = true;        // set the flag to true
                    DEBUG_PRINT("Player added to existing session as player2\n");
                    cout << INFO << "Player added to existing session as player2" << RESET << endl;
                    break; // exit the loop since we found a session
                }
                // else just continue to the next session
            }
        }
        if (!inside_session)
        {
            // if the player is not added to an existing session, create a new session
            lock_guard<mutex> lock(all_sessions_mutex);
            Session *new_session = new Session(player, nullptr); // create a new session with the player as player1
            ALL_SESSIONS.push_back(new_session);                 // add the new session to the global list of sessions
            player->set_session(new_session);                    // set the session for the player object
            DEBUG_PRINT("New session created for player\n");
            cout << INFO << "New session created for player" << RESET << endl;
            cout << INFO << "Waiting for another player to join..." << RESET << endl;
        }
    }
    else
    {
        cerr << ERROR << "Unknown game mode chosen by client: " << player->get_chosen_game_mode() << RESET << endl;
        close(client_sock);
        return;
    }

    Session *ptr_curr_sess = player->get_session(); // pointer to the current session of the player,

#pragma endregion

#pragma region Start Game Thread
    /* ---------------- Start game thread --------------------------*/
    DEBUG_PRINT("------------------ Starting game thread -----------------\n");
    // The client chose to play against AI
    if (player->get_chosen_game() == 0 && player->get_chosen_game_mode() == 1)
    {
        // The client should already be in a full session with the AI
        // Send the client a message that the game is starting
        if (send_to(client_sock, "WAITING_END") < 0)
        {
            cerr << ERROR << "Error sending game start message to client: " << strerror(errno) << RESET << endl;
            close(client_sock);
            return;
        }
        DEBUG_PRINT("sent WAITING_END\n");
        cout << INFO << "Starting MCTS checkers game against AI" << RESET << endl;
        thread mcts_thread(gameplay_ai, player); // start the MCTS checkers game against AI in a separate thread
        mcts_thread.join();                      // wait for the MCTS thread to finish

        // delete the session after the game is finished
        {
            lock_guard<mutex> lock(all_sessions_mutex);
            for (auto it = ALL_SESSIONS.begin(); it != ALL_SESSIONS.end();)
            {
                if ((*it)->player1 == player)
                {
                    // if the player is player1, delete the session
                    DEBUG_PRINT("Deleting AI session\n");
                    delete *it;                   // delete the session
                    it = ALL_SESSIONS.erase(it);  // remove the session from the global list of sessions
                    player->set_session(nullptr); // set the session for the player object to nullptr
                    break;
                }
                else
                {
                    it++;
                }
            }
        }

        cout << SUCCESS << "MCTS checkers game against AI finished" << RESET << endl;
    }
    // The client chose to play against another player
    else if (player->get_chosen_game() == 0 && player->get_chosen_game_mode() == 0)
    {
        int not_full = ptr_curr_sess->is_full();
        int *ptr_not_full = &not_full;
        thread waiting_thread(waiting_message, "Waiting for other players", INFO, ptr_not_full);
        // wait until the session is full
        while (ptr_curr_sess->is_full() == false)
        {
            sleep(1); // sleep for 1 second to avoid busy waiting
        }
        not_full = 0;          // set the not_full flag to 0 to stop the waiting thread
        waiting_thread.join(); // wait for the waiting thread to finish

        // only player1's thread should start the game
        if (ptr_curr_sess->player1 == player)
        {
            cout << INFO << "Session is full, starting multiplayer checkers game" << RESET << endl;

            if (send_to(ptr_curr_sess->player2->get_socket(), "WAITING_END") < 0)
            {
                cerr << ERROR << "Error sending game start message to client: " << strerror(errno) << RESET << endl;
                close(client_sock);
                return;
            }

            if (send_to(client_sock, "WAITING_END") < 0)
            {
                cerr << ERROR << "Error sending game start message to client: " << strerror(errno) << RESET << endl;
                close(client_sock);
                return;
            }

            thread multiplayer_thread(gameplay_multiplayer, ptr_curr_sess);
            multiplayer_thread.join(); // wait for the multiplayer thread to finish
            // delete the session after the game is finished
            {
                lock_guard<mutex> lock(all_sessions_mutex);
                for (auto it = ALL_SESSIONS.begin(); it != ALL_SESSIONS.end();)
                {
                    if (*it == ptr_curr_sess)
                    {
                        // if the player is player1, delete the session
                        DEBUG_PRINT("Deleting Multiplayer session\n");
                        (*it)->player1->set_session(nullptr); // notify player1's thread to exit
                        (*it)->player2->set_session(nullptr); // notify player2's thread to exit
                        delete *it;                           // delete the session
                        it = ALL_SESSIONS.erase(it);          // remove the session from the global list of sessions
                        break;
                    }
                    else
                    {
                        it++;
                    }
                }
            }
        }
        else
        {
            // player2's thread waits for the game to be over
            while (player->get_session() != nullptr)
            {
                sleep(1);
            }
        }

        cout << SUCCESS << "Multiplayer checkers game finished" << RESET << endl;
    }
    else
    {
        cerr << ERROR << "Unknown game or game mode chosen by client" << RESET << endl;
    }
    /* ---------------- End Start game thread --------------------------*/

    /* ---------------- close socket --------------------------*/
    close(client_sock);
    cout << INFO << "Client socket closed" << RESET << endl;
    /* ---------------- End close socket --------------------------*/
    DEBUG_PRINT("Thread finished for client socket: " + to_string(client_sock) + "\n");
#pragma endregion

    return;
}
#pragma endregion

#pragma region AI Gameplay
// ------ USER INTERACTION MODE -----
int gameplay_ai(Player *player)
{
    int player_socket = player->get_socket();
    Session *ptr_session = player->get_session();
    /* variable to store who won the game
       -1 - no one surrendered
        0 - player1 surrendered
        1 - player2/AI surrendered
    */
    int surrendered = -1;

    // send "GAME_START" to the client

#pragma region Load MCTS Tree
    // load the tree from file and reconstruct tree
    MCTS_leaf *mcts_tree = load_or_create_mcts_tree();
#pragma endregion
    DEBUG_PRINT("MCTS tree loaded successfully\n");

#pragma region Send which player they are
    ptr_session->player1->set_id(1); // set player 1 id
    // send to the client which player they are
    string player_id_str = "You are player " + to_string(ptr_session->player1->get_id()) + ".";
    DEBUG_PRINT("Sending player id to client: " + player_id_str + "\n");
    if (send_to(player->get_socket(), "TEXT", player_id_str) < 0)
    {
        cerr << ERROR << "Error sending player id to client: " << strerror(errno) << RESET << endl;
        close(player_socket);
        return save_and_exit(mcts_tree);
    }
#pragma endregion
    DEBUG_PRINT("Player id sent to client successfully\n");

#pragma region Game Loop
    // Game loop
    MCTS_leaf *current_node = mcts_tree;
    ptr_session->curr_state = current_node->state; // copy the session from the current node to the player session
    int tmpres = -1;                               // variable to store the result of the send_to function
    bool want_to_play_on = true;                   // flag to check if the players want to play again
    while (want_to_play_on)
    {

        while (true)
        {
            DEBUG_PRINT("Inside game loop\n");
            // populate possible moves for the current node
            current_node->state.list_all_possible_moves(current_node->state.get_current_player());
            // if current node is terminal, end the game
            int terminal = current_node->state.TerminalState();
            if (terminal != -1)
            {
                ptr_session->curr_state.get_board()->print_Board(); // print the board for debugging purposes
                DEBUG_PRINT("Game is over, terminal state reached\n");
                string string_to_send = "Game over! Player " + to_string(terminal) + " won!";
                send_to(player->get_socket(), "CHECKERS_END", string_to_send);

                // ask if the players want to play again
                string question = "Do you want to play again? (yes/no)";
                if (send_to(player->get_socket(), "QUESTION_STR", question) < 0)
                {
                    cerr << ERROR << "Error sending question to player 1: " << strerror(errno) << RESET << endl;
                    want_to_play_on = false; // set the flag to false to exit the game loop
                }
                string answer1;
                string *ptr_answer1 = &answer1;

                get_response(player->get_socket(), ptr_answer1);
                if (answer1 == "Yes")
                {
                    {
                        // if player wants to play again, save the mcts tree to file
                        save_and_exit(mcts_tree); // save the MCTS tree to file
                        // restart the game
                        DEBUG_PRINT("MCTS tree saved to file\n");
                        // reset the game state
                        ptr_session->curr_state = load_or_create_mcts_tree()->state; // reset the game state to the initial state
                    }
                    ptr_session->current_player = player; // reset the current player to player1
                    continue;                             // continue the game loop
                }
                else
                {
                    want_to_play_on = false; // set the flag to false to exit the game loop
                }
                send_to(player->get_socket(), "GOODBYE");
                break; // exit the game loop
            }
            // Player's turn
            if (current_node->state.get_current_player() == player->get_id())
            {
                sleep(1);    // sleep for 1 second to give the player time to read the message
                tmpres = -1; // reset tmpres for the next send_to

                /* ------------------- send gamestate (board and moves) ------------------- */
                // encode gamestate to string
                string gamestate_str = current_node->state.get_state_info();
                string board_str = current_node->state.get_board()->get_board_info();
                while (tmpres == -1)
                {
                    tmpres = send_to(player->get_socket(), "CHECKERS_STATE", gamestate_str);
                }
                DEBUG_PRINT("Sent gamestate to player\n");
                tmpres = -1; // reset tmpres for the next send_to
                // send_to(player->get_socket(), "CHECKERS_STATE", gamestate_str);
                /* ------------------- store move from client in "prev_move" ------------------- */
                get_response(player_socket, ptr_session);
                if (ptr_session->quit_requested)
                {
                    send_to(player->get_socket(), "GOODBYE", "oooooooooooooooooooooo\n");
                    DEBUG_PRINT("Player requested to quit the game\n");
                    return save_and_exit(mcts_tree);
                }
                /* ------------------- perform move on the gamestate ------------------- */
                // perform the move by searching the children of the current node and finding the one that matches the move
                string selected_move_str = ptr_session->prev_move.get_move_info();
                // select a move that has not been explored yet
                // load all of the children into a set
                map<string, MCTS_leaf *> moves_children;
                for (MCTS_leaf *child : current_node->children)
                {
                    moves_children.insert({child->get_move_info(), child});
                }
                // check if this move has already been explored by the AI
                bool found = false;
                for (Move move : current_node->state.possible_moves)
                {
                    // map.find() returns an iterator to the element if found, or end() if not found
                    auto it = moves_children.find(selected_move_str);
                    if (it != moves_children.end())
                    {
                        // if the child move matches, we can select it
                        found = true;
                        current_node = it->second;
                        break;
                    }
                }
                if (!found)
                {
                    Move selected_move = ptr_session->prev_move; // get the move from the session
                    // if the AI has not explored this move yet, we need to create a new child node
                    // create a new game state with the selected move
                    GameState new_game_state = current_node->state.clone();
                    // change the player of the new game state
                    new_game_state.switch_player();
                    // perform move
                    Board *tmp_board = new_game_state.get_board();
                    selected_move.perform_move(tmp_board, selected_move);
                    // populate the possible moves of the new game state
                    new_game_state.list_all_possible_moves(new_game_state.get_current_player());
                    // create a new child node with the new game state and add to the tree
                    MCTS_leaf *new_child = new MCTS_leaf(new_game_state, selected_move, current_node);
                    current_node->children.push_back(new_child);
                    current_node = new_child;
                    // update the session with the new game state
                    ptr_session->curr_state = current_node->state; // copy the session from the current node to the player session

                    // train the AI on this new node
                    train(current_node, 20);
                }
            }
            else
            {
                // AI's turn
                DEBUG_PRINT("AI's turn!\n");
                // AI will play
                // select the best move from the MCTS tree
                MCTS_leaf *newnode = select_most_visited_child(current_node);
                // if newnode is current_node
                if (newnode == current_node)
                {
                    // we are not at the terminal state,
                    // which means the AI has not expolred this part of the tree yet.
                    // so we need to expand the tree by training the ai
                    train(newnode, 30);
                    // select the best child of the new node
                    newnode = select_most_visited_child(newnode);
                }
                current_node = newnode;
                // update the game session
                ptr_session->curr_state = current_node->state; // copy the session from the new node to the player session
            }
        }
    }
#pragma endregion

    return save_and_exit(mcts_tree);
}
#pragma endregion

#pragma region Multiplayer Gameplay
int gameplay_multiplayer(Session *sess) // arguments TBD
{
#pragma region Initialize
    // init helper variables
    int player1_socket = sess->player1->get_socket();
    int player2_socket = sess->player2->get_socket();
    Player *player1 = sess->player1;
    Player *player2 = sess->player2;

    // Create a default Board and GameState
    Board board = array<array<Piece, 8>, 8>{{{Piece(NOPLAYER, 0, 0), Piece(PLAYER1, 0, 1), Piece(NOPLAYER, 0, 2), Piece(PLAYER1, 0, 3), Piece(NOPLAYER, 0, 4), Piece(PLAYER1, 0, 5), Piece(NOPLAYER, 0, 6), Piece(PLAYER1, 0, 7)},
                                             {Piece(PLAYER1, 1, 0), Piece(NOPLAYER, 1, 1), Piece(PLAYER1, 1, 2), Piece(NOPLAYER, 1, 3), Piece(PLAYER1, 1, 4), Piece(NOPLAYER, 1, 5), Piece(PLAYER1, 1, 6), Piece(NOPLAYER, 1, 7)},
                                             {Piece(NOPLAYER, 2, 0), Piece(PLAYER1, 2, 1), Piece(NOPLAYER, 2, 2), Piece(PLAYER1, 2, 3), Piece(NOPLAYER, 2, 4), Piece(PLAYER1, 2, 5), Piece(NOPLAYER, 2, 6), Piece(PLAYER1, 2, 7)},
                                             {Piece(NOPLAYER, 3, 0), Piece(NOPLAYER, 3, 1), Piece(NOPLAYER, 3, 2), Piece(NOPLAYER, 3, 3), Piece(NOPLAYER, 3, 4), Piece(NOPLAYER, 3, 5), Piece(NOPLAYER, 3, 6), Piece(NOPLAYER, 3, 7)},
                                             {Piece(NOPLAYER, 4, 0), Piece(NOPLAYER, 4, 1), Piece(NOPLAYER, 4, 2), Piece(NOPLAYER, 4, 3), Piece(NOPLAYER, 4, 4), Piece(NOPLAYER, 4, 5), Piece(NOPLAYER, 4, 6), Piece(NOPLAYER, 4, 7)},
                                             {Piece(PLAYER2, 5, 0), Piece(NOPLAYER, 5, 1), Piece(PLAYER2, 5, 2), Piece(NOPLAYER, 5, 3), Piece(PLAYER2, 5, 4), Piece(NOPLAYER, 5, 5), Piece(PLAYER2, 5, 6), Piece(NOPLAYER, 5, 7)},
                                             {Piece(NOPLAYER, 6, 0), Piece(PLAYER2, 6, 1), Piece(NOPLAYER, 6, 2), Piece(PLAYER2, 6, 3), Piece(NOPLAYER, 6, 4), Piece(PLAYER2, 6, 5), Piece(NOPLAYER, 6, 6), Piece(PLAYER2, 6, 7)},
                                             {Piece(PLAYER2, 7, 0), Piece(NOPLAYER, 7, 1), Piece(PLAYER2, 7, 2), Piece(NOPLAYER, 7, 3), Piece(PLAYER2, 7, 4), Piece(NOPLAYER, 7, 5), Piece(PLAYER2, 7, 6), Piece(NOPLAYER, 7, 7)}}};
    // default board
    GameState game_state(board, PLAYER1);
    sess->curr_state = game_state; // set the current game state to the session
#pragma endregion
    // // Initialize current_player to player1 (since the game starts with player1)
    // sess->current_player = player1;

#pragma region Send which player they are
    player1->set_id(1); // set player 1 id
    player2->set_id(2); // set player 2 id
    // send to player 1
    string player_id_str = "You are player " + to_string(player1->get_id()) + ".";
    DEBUG_PRINT("Sent player id to client: " + player_id_str + "\n");

    // send to player 2
    if (send_to(player1_socket, "TEXT", player_id_str) < 0)
    {
        cerr << ERROR << "Error sending player id to client: " << strerror(errno) << RESET << endl;
        close(player1_socket);
        return -1;
    }
    player_id_str = "You are player " + to_string(player2->get_id()) + ".";
    DEBUG_PRINT("Sending player id to client: " + player_id_str + "\n");
    if (send_to(player2_socket, "TEXT", player_id_str) < 0)
    {
        cerr << ERROR << "Error sending player id to client: " << strerror(errno) << RESET << endl;
        close(player2_socket);
        return -1;
    }
#pragma endregion

    sleep(1); // sleep for 1 second for mysterious reasons

    bool want_to_play_on = true; // flag to check if the players want to play on
#pragma region Game Loop
    while (want_to_play_on)
    {
        while (true)
        {
            DEBUG_PRINT("Inside game loop\n");
            // populate possible moves for the current node
            sess->curr_state.list_all_possible_moves(sess->curr_state.get_current_player());
            // if current node is terminal, end the game
            int terminal = sess->curr_state.TerminalState();
            DEBUG_PRINT("Terminal state: " + to_string(terminal) + "\n");
            if (terminal != -1)
            {
                sess->curr_state.get_board()->print_Board(); // print the board for debugging purposes
                DEBUG_PRINT("Game is over, terminal state reached\n");
                string string_to_send = "Game over! Player " + to_string(terminal) + " won!";
                send_to(player1_socket, "CHECKERS_END", string_to_send);
                send_to(player2_socket, "CHECKERS_END", string_to_send);

                // ask if the players want to play again
                string question = "Do you want to play again? (yes/no)";
                if (send_to(player1_socket, "QUESTION_STR", question) < 0)
                {
                    cerr << ERROR << "Error sending question to player 1: " << strerror(errno) << RESET << endl;
                    want_to_play_on = false; // set the flag to false to exit the game loop
                }
                if (send_to(player2_socket, "QUESTION_STR", question) < 0)
                {
                    cerr << ERROR << "Error sending question to player 2: " << strerror(errno) << RESET << endl;
                    want_to_play_on = false; // set the flag to false to exit the game loop
                }
                string answer1;
                string *ptr_answer1 = &answer1;
                string answer2;
                string *ptr_answer2 = &answer2;

                get_response(player1_socket, ptr_answer1);
                get_response(player2_socket, ptr_answer2);
                if (answer1 == "Yes" && answer2 == "Yes")
                {
                    // if both players want to play again, reset the game state
                    sess->curr_state = GameState(board, PLAYER1); // reset the game state
                    sess->current_player = player1;               // reset the current player to player1
                    continue;                                     // continue the game loop
                }
                else
                {
                    want_to_play_on = false; // set the flag to false to exit the game loop
                }
                send_to(player1_socket, "GOODBYE");
                send_to(player2_socket, "GOODBYE");
                break; // exit the game loop
            }
            // Player's turn
            Player *current_player_obj = sess->current_player;
            DEBUG_PRINT("Current player: " + to_string(sess->current_player->get_id()) + "\n");

            /* ------------------- send gamestate (board and moves) ------------------- */
            // encode gamestate to string
            string gamestate_str = sess->curr_state.get_state_info();
            if (send_to(sess->current_player->get_socket(), "CHECKERS_STATE", gamestate_str) < 0)
            {
                cerr << ERROR << "Error sending state to player " << sess->current_player->get_id() << RESET << endl;
                return -1;
            }
            DEBUG_PRINT("Sent gamestate to player\n");

            /* ------------------- store move from client in "prev_move" ------------------- */
            get_response(sess->current_player->get_socket(), sess);
            if (sess->quit_requested)
            {
                send_to(player1_socket, "GOODBYE", "player " + to_string(sess->quit_requested) + " requested to quit\n");
                send_to(player2_socket, "GOODBYE", "player " + to_string(sess->quit_requested) + " requested to quit\n");
                return 0; // quit game
            }
            /* ------------------- perform move on the gamestate ------------------- */
            // perform the move by searching the children of the current node and finding the one that matches the move
            sess->curr_state.get_board()->perform_move(sess->prev_move);
            sess->curr_state.switch_player();
            // switch the current player in the session
            sess->current_player = (sess->current_player == player1) ? player2 : player1;
        }
    }
#pragma endregion
    return 0;
}
#pragma endregion