#include "request_helpers.hpp"

using namespace std;

// Global variables for lobby state
int LOBBY_MODE = 1;

inline void clear_screen()
{
    (void)system("clear");
}

int init_server_sock()
{
    /* init socket
     * This creates a TCP socket that the server will use to listen for incoming connections.
     */
    int sock = socket(AF_INET, SOCK_STREAM, 0); // create a TCP socket for listening for incoming connections
    if (sock < 0)
    {
        cerr << ERROR << "Error creating socket: " << strerror(errno) << RESET << endl;
        return -1;
    }
    DEBUG_PRINT("Socket created successfully, now we can listen to incoming connections\n");

    /* specify connection
     * It is the connection (ports, addresses, etc.) that the server will use to accept incoming connections.
     */
    struct sockaddr_in server_service;
    server_service.sin_addr.s_addr = INADDR_ANY; // allow any ipv4 address
    server_service.sin_family = AF_INET;         // IPv4
    server_service.sin_port = htons(PORT);       // listen on port PORT
    DEBUG_PRINT("Connection specified: " + string(inet_ntoa(server_service.sin_addr)) + ":" + to_string(ntohs(server_service.sin_port)) + "\n");

    /* Bind socket to port
     * This binds the socket to the specified service.
     * It allows the server to listen for incoming connections on that port.
     */
    int res = bind(sock, (struct sockaddr *)&server_service, sizeof(server_service));
    if (res < 0)
    {
        cerr << ERROR << "Error binding socket: " << strerror(errno) << RESET << endl;
        close(sock);
        return -1;
    }
    DEBUG_PRINT("--------- Server is ready to accept connections ---------\n");

    /* listen for incoming connections
     * This tells the socket to listen for incoming connections.
     */
    res = listen(sock, SOMAXCONN);
    if (res < 0)
    {
        cerr << ERROR << "Error listening on socket: " << strerror(errno) << RESET << endl;
        close(sock);
        return -1;
    }
    DEBUG_PRINT("Socket is now listening for incoming connections\n");
    cout << INFO << "Server is listening on port " << PORT << RESET << endl;

    return sock;
}

int init_client_sock()
{
    cout << "Client started" << endl;
    // init socket
    int sock = socket(AF_INET, SOCK_STREAM, 0); // create a TCP socket for connecting to a server
    if (sock < 0)
    {
        cerr << ERROR << "Error creating socket: " << strerror(errno) << RESET << endl;
        return -1;
    }
    DEBUG_PRINT("Socket created successfully\n");

    // define target address
    struct sockaddr_in ip4addr; // destination address
    string dest_ip;
    bool valid = false;
    while (!valid)
    {
        cout << BOLDYELLOW << "Enter the server IP address [default: 127.0.0.1]: " << RESET;
        cin >> dest_ip; // read the IP address from user input
        // validate the IP address
        if (dest_ip.empty())
        {
            cout << INFO << "Using default: localhost" << RESET << endl;
            dest_ip = "127.0.0.1";
        }
        else if (dest_ip.length() > 15 || dest_ip.length() < 7)
        {
            cerr << ERROR << "Invalid IP address length. Please enter a valid IPv4 address." << RESET << endl;
            continue;
        }
        else if (dest_ip.find_first_not_of("0123456789.") != string::npos)
        {
            cerr << ERROR << "Invalid characters in IP address. Please enter a valid IPv4 address." << RESET << endl;
            continue;
        }

        // split the IP address into octets
        int pos = 0;
        int octet_count = 0;
        string dest_ip_to_check = dest_ip + '.'; // append a dot to the end to handle the last octet
        size_t dot_pos = dest_ip_to_check.find('.');
        while (dot_pos != string::npos && octet_count < 5)
        {
            valid = true;
            string octet = dest_ip_to_check.substr(pos, dot_pos - pos); // get the octet
            if (octet.empty() || stoi(octet) < 0 || stoi(octet) > 255)
            {
                cerr << ERROR << "Invalid octet: " << octet << ". Each octet must be between 0 and 255." << RESET << endl;
                valid = false; // set valid to false to continue the loop
                break;         // exit the loop if an invalid octet is found
            }
            pos = dot_pos + 1;                         // move to the next position after the dot
            dot_pos = dest_ip_to_check.find('.', pos); // find the next dot
            octet_count++;
        }
        if (!valid)
            continue;
        valid = true;
    }

    inet_pton(AF_INET, dest_ip.c_str(), &ip4addr.sin_addr);
    cout << INFO << "Target address set: " << dest_ip << RESET << endl;

    // set up socket address
    struct sockaddr_in clientService;          // client address
    clientService.sin_family = AF_INET;        // IPv4
    clientService.sin_port = htons(PORT);      // port number
    clientService.sin_addr = ip4addr.sin_addr; // bind socket to the target address
    cout << INFO << "Socket address set: " << dest_ip << ":" << PORT << RESET << endl;
    // DEBUG_PRINT("Socket address set: " + dest_ip + ":" + to_string(PORT) + "\n");

    // connect to server
    int iResult = connect(sock, (struct sockaddr *)&clientService, sizeof(clientService));
    if (iResult < 0)
    {
        cerr << ERROR << "Error connecting to server: " << strerror(errno) << RESET << endl;
        close(sock);
        return -1;
    }
    cout << SUCCESS << "Connected to server at " << dest_ip << ":" << PORT << RESET << endl;

    return sock;
}

int send_to(int sock, const string opcode, const string message)
{
    string tosend = opcode + ":<" + message + ">" + CONTROL_CHAR;
    DEBUG_PRINT("Sending message to server: " + tosend + "\n");
    // send message to client
    ssize_t bytes_sent = send(sock, tosend.c_str(), tosend.length(), 0);
    if (bytes_sent < 0)
    {
        cerr << ERROR << "Error sending message to server: " << strerror(errno) << RESET << endl;
        return -1;
    }
    return 0; // success
}

int send_to(int sock, const string opcode)
{
    string tosend = opcode + ":" + "<>" + CONTROL_CHAR;
    // send message to client
    ssize_t bytes_sent = send(sock, tosend.c_str(), tosend.length(), 0);
    if (bytes_sent < 0)
    {
        cerr << ERROR << "Error sending message to server: " << strerror(errno) << RESET << endl;
        return -1;
    }
    return 0; // success
}

string receive_from(int sock)
{
    int res = 0;               // initialize res to 0
    string received_data = ""; // string to hold received data
    bool cc_found = false;     // flag to check if control character is found
    while (cc_found == false)  // continue until control character is found
    {
        char buffer[BUFFER_SIZE];
        int res = recv(sock, &buffer, BUFFER_SIZE, 0); // receive data
        if (res > 0)
        {
            // successful in receiving data
            for (int i = 0; i < res; i++)
            {
                // concat to string
                received_data += buffer[i];
                if (buffer[i] == CONTROL_CHAR) // check for control character
                {
                    // stop receiving data when control character is found
                    cc_found = true; // set flag to true
                    break;
                }
            }
        }
        else if (res < 0)
        {
            // e.g. -1: Error
            cerr << ERROR << "Error receiving data: " << strerror(errno) << RESET << endl;
            throw runtime_error("Error receiving data from client");
        }
        else
        {
            // no more data: done
            break;
        }
    }
    DEBUG_PRINT("Received data: " + received_data + "\n");
    return received_data;
}

int handshake(int &sock)
{
    // send a handshake message to the server
    string handshake_message = "Hello, server!";
    DEBUG_PRINT("Sending handshake message: " + handshake_message + "\n");
    if (send_to(sock, "TEXT", handshake_message) < 0)
    {
        cerr << ERROR "Failed to send handshake message to server" << RESET << endl;
        return -1;
    }
    DEBUG_PRINT("Handshake message sent successfully\n");

    // receive a response from the server
    string response = receive_from(sock);
    if (response.empty())
    {
        cerr << ERROR "No response received from server" << RESET << endl;
        return -1;
    }
    DEBUG_PRINT("Received response from server: " + response + "\n");

    // check if the response matches the handshake message
    if (handshake_message != parsed_response(response)["data"].front())
    {
        cerr << ERROR "Handshake failed: response does not match handshake message" << RESET << endl;
        return -1;
    }

    int i = send_to(sock, "PERFECT"); // send a confirmation message back to the server
    if (i != 0)
    {
        cerr << ERROR "Failed to send confirmation message to server" << RESET << endl;
        return -1;
    }

    cout << SUCCESS "Handshake successful!" << RESET << endl;

    return 0; // success
}

bool opcode_is_valid(const string opcode)
{
    return opcode == "HEY" || opcode == "PERFECT" || opcode == "TEXT" || opcode == "WAITING" || opcode == "WAITING_END" || opcode == "OK" || opcode == "QUESTION" || opcode == "QUESTION_STR" || opcode == "ANSWER" || opcode == "CHECKERS_STATE" || opcode == "CHECKERS_MOVES" || opcode == "CHECKERS_END" || opcode == "GOODBYE";
}

bool opcode_is_valid(const string opcode, const string type)
{
    if (type == "empty data allowed")
        return opcode == "HEY" || opcode == "PERFECT" || opcode == "WAITING_END" || opcode == "OK";
    else
        return opcode == "HEY" || opcode == "PERFECT" || opcode == "TEXT" || opcode == "WAITING" || opcode == "WAITING_END" || opcode == "OK" || opcode == "QUESTION" || opcode == "QUESTION_STR" || opcode == "ANSWER" || opcode == "CHECKERS_STATE" || opcode == "CHECKERS_MOVES" || opcode == "CHECKERS_END" || opcode == "GOODBYE";
}

map<string, vector<string>> parsed_response(string response)
{
    vector<string> data_list; // vector to hold data
    vector<string> type_list; // vector to hold type (can be empty or have a value)
    map<string, vector<string>> parsed_map = {{"type", type_list}, {"data", data_list}};

    // DEBUG_PRINT("Parsing response: " + response + "\n");
    DEBUG_PRINT("Parsing response...\n");

    // parse the response string
    size_t pos = 0;
    // remove the trailing control characters
    size_t cc_pos = response.find(CONTROL_CHAR);
    if (cc_pos == string::npos)
        throw runtime_error("No control characters found in received data");

    response = response.substr(0, cc_pos); // truncate the string at the first control character

    // Split the response string by colon (:) to get tokens.
    // The format is: <op-code>:<data>
    /*
    ********************************
    get the op-code
    ********************************
    */
    pos = response.find(':');
    string part = response.substr(0, pos);

    // reove the part from the response string
    response.erase(0, pos + 1);

    // this is the opcode of the response
    // check if the part is valid
    if (part.empty())
    {
        throw runtime_error("Invalid response format: no opcode found");
    }
    else if (!opcode_is_valid(part))
    {
        throw runtime_error("Invalid response format: unknown opcode '" + part + "'");
    }
    parsed_map["type"].push_back(part); // add the opcode to the type list
    bool opcode_type_empty_header = opcode_is_valid(part, "empty data allowed");

    /*
    ********************************
    now we parse the data part (in a while loop)
    ********************************
    */
    // the data is enclosed in <> brackets
    pos = response.find('<');
    if (pos == string::npos)
    {
        // if there is no '<' in the response, it is malformed
        throw runtime_error("Invalid response format: no opening '<' found");
    }
    response.erase(0, pos + 1); // remove the part before the first '<'
    pos = response.find('>');
    if (pos == string::npos)
    {
        throw runtime_error("Invalid response format: no closing '>' found");
    }
    string data_part = response.substr(0, pos); // get the data part
    response.erase(0, pos + 1);                 // remove the part before the first '>'

    if (opcode_type_empty_header && !data_part.empty())
    {
        // if the data part is HEY, PERFECT, GOODBYE or OK and the type is not empty, it is malformed
        throw runtime_error("Invalid response format: data part '" + data_part + "' is not expected for opcode '" + part + "'");
    }
    else if (opcode_type_empty_header && data_part.empty())
    {
        // if the data part is HEY, PERFECT, GOODBYE or OK and the type is empty, it is correct and we can return
        DEBUG_PRINT("Data part is empty, ignoring it for " + part + " response\n");
        parsed_map["data"].clear(); // clear the data vector
        return parsed_map;          // return the parsed map as it is
    }
    else if (!data_part.empty() && opcode_is_valid(part))
    {
        // if the data part is not empty, we parse it
        // DEBUG_PRINT("Parsing data part: " + data_part + "\n");

        // split the data part by semicolon (;) to get the individual data items
        size_t start = 0;
        size_t end = data_part.find(';');
        while (end != string::npos)
        {
            string item = data_part.substr(start, end - start);
            if (!item.empty())
            {
                parsed_map["data"].push_back(item); // add the item to the data vector
                // DEBUG_PRINT("Parsed data item: " + item + "\n");
            }
            start = end + 1;
            end = data_part.find(';', start);
        }

        // add the last item (if any)
        if (start < data_part.length())
        {
            parsed_map["data"].push_back(data_part.substr(start));
            // DEBUG_PRINT("Parsed last data item: " + data_part.substr(start) + "\n");
        }
    }
    else
    {
        throw runtime_error("Invalid response format: no data found");
    }

    return parsed_map;
}

GameState decode_state_response(string data)
{
    // remove "[" from the beginning of the string and "]" from the end
    if (data.substr(0, 1) != "[")
    {
        throw runtime_error("Invalid response format: expected '[' at the beginning of the string");
    }
    if (data.back() != ']')
    {
        throw runtime_error("Invalid response format: expected ']' at the end of the string");
    }
    data.erase(0, 1);                 // remove "["
    data.erase(data.length() - 1, 1); // remove the last "]"

    /* it will be in the format: "b[p[...],p[...]],curr_player],wins,total_games,rating,is_terminal]" */
    // get board (go until 'p')
    data.erase(0, 2);    // erase the first 2 characters (b[)        // extract the board string
    int tmp_counter = 1; // holds counter for bracket, so we can find mathcing bracket
    int i = 0;           // index for the string
    // parse until counter is 0
    while (tmp_counter != 0)
    {
        if (data[i] == '[')
        {
            tmp_counter++;
        }
        else if (data[i] == ']')
        {
            tmp_counter--;
        }
        i++;
    }
    int board_end = i; // end of the board (includes the last bracket; "...]]")

    string board_str = data.substr(0, board_end); // board_str will have the format p[...],...,p[...]]
    // parse the board and create a new board object
    array<array<Piece, 8>, 8> new_board_data;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // erease the first 2 characters (p[)
            board_str.erase(0, 2);
            /* Each piece will now have the format: "...],p[...],...,p[...]]" */
            // get the player id
            int player_id = stoi(board_str.substr(0, board_str.find(',')));
            board_str.erase(0, board_str.find(',') + 1);
            // get the y coordinate
            int y = stoi(board_str.substr(0, board_str.find(',')));
            board_str.erase(0, board_str.find(',') + 1);
            // get the x coordinate
            int x = stoi(board_str.substr(0, board_str.find(',')));
            board_str.erase(0, board_str.find(',') + 1);
            // get the is_king
            bool is_king = stoi(board_str.substr(0, board_str.find(']'))) == 0 ? false : true;
            board_str.erase(0, board_str.find(']') + 2);
            // create a new piece object
            Piece new_piece(player_id, y, x, is_king);
            // set the piece in the new board
            new_board_data[i][j] = new_piece;
        }
    }
    // create a new board object
    Board new_board_obj(new_board_data);
    data.erase(0, board_end + 1);

    /* now the string data reads "curr_player],wins,total_games,rating,is_terminal,is_computer]" */
    int curr_player = stoi(data.substr(0, data.find(']')));
    // create a new game state object
    GameState new_game_state(new_board_obj, curr_player);
    new_game_state.list_all_possible_moves(curr_player); // list all possible moves for the current player

    data.erase(0, data.find(']') + 2); // erease the last bracket and comma

    return new_game_state; // return the new game state object
}

Move decode_move_response(string data)
{
    // remove "m[" from the beginning of the string and "]" from the end
    if (data.substr(0, 1) != "[")
    {
        throw runtime_error("Invalid response format: expected '[' at the beginning of the string");
    }
    if (data.back() != ']')
    {
        throw runtime_error("Invalid response format: expected ']' at the end of the string");
    }
    data.erase(0, 1);                 // remove "["
    data.erase(data.length() - 1, 1); // remove the last "]"

    // now we have a string in the format: "src_y,src_x,dest_y,dest_x,jump,enemy_y,enemy_x"
    vector<string> move_data;
    size_t pos = 0;
    while ((pos = data.find(',')) != string::npos)
    {
        move_data.push_back(data.substr(0, pos));
        data.erase(0, pos + 1);
    }
    move_data.push_back(data); // add the last part

    if (move_data.size() != 7)
    {
        throw runtime_error("Invalid response format: expected 7 values for move");
    }

    // create a new move object
    Move new_move(stoi(move_data[0]), stoi(move_data[1]), stoi(move_data[2]), stoi(move_data[3]), stoi(move_data[4]) == 1, stoi(move_data[5]), stoi(move_data[6]));

    return new_move; // return the new move object
}

void process_response(const map<string, vector<string>> parsed_response, int socket, int *flag_to_set)
{
    // Process the parsed response based on the opcode
    string opcode = parsed_response.at("type").front();
    vector<string> data = parsed_response.at("data");

    DEBUG_PRINT("Processing response with opcode: " + opcode + "\n");

    if (opcode == "CHECKERS_STATE")
    {
        // The checkers board turn response needs another (overloaded) function to process it
        DEBUG_PRINT("Received CHECKERS_STATE response, decoding and printing...\n");
        GameState curr_state = decode_state_response(data.front());
        curr_state.get_board()->print_Board(); // print the board
        // print if player 1 or player 2 is the current player
        if (curr_state.get_current_player() == PLAYER1)
        {
            cout << BOLDYELLOW << "Current player: Player 1" << RESET << endl;
        }
        else if (curr_state.get_current_player() == PLAYER2)
        {
            cout << BOLDYELLOW << "Current player: Player 2" << RESET << endl;
        }
        else
        {
            cout << ERROR << "Unknown current player!" << RESET << endl;
        }
        // print list of possible moves
        curr_state.print_all_moves();
        // receive the question
        string input;
        while (true)
        {
            cout << "Select a move by number or [q] to quit: ";
            cin >> input; // read the input from the user

            if (input == "q")
            {
                // if the input is 'q', send a CHECKERS_END request to the server
                send_to(socket, "CHECKERS_MOVES", "q"); // send a surrender request
                cout << BOLDYELLOW << "You surrendered!!\n" << RESET << endl;
                // close the socket and exit the program TODO: close the socket properly
                // *flag_to_set = 1; // set the flag to true
                // exit(0); // exit the program
                return;
            }
            else if (stoi(input) <= curr_state.num_possible_moves() && stoi(input) > 0)
            {
                // if the input is a valid move number
                // select move from the state
                Move selected_move = curr_state.possible_moves.at(stoi(input) - 1); // get the selected move
                string move_info = selected_move.get_move_info();                   // get the move info

                // send the move info to the server
                if (send_to(socket, "CHECKERS_MOVES", move_info) < 0)
                {
                    cerr << ERROR << "Failed to send move to server" << RESET << endl;
                    return;
                }
                DEBUG_PRINT("Sent move to server: " + move_info + "\n");
            }
            else
            {
                cerr << ERROR << "Invalid input, please try again." << RESET << endl;
                // DEBUG_PRINT("Invalid input received: " + input + "\n");
                // DEBUG_PRINT("Expected input: a number between 1 and " + to_string(curr_state.num_possible_moves()) + " or 'q' to quit\n");
                continue;
            }
            break;
        }
    }
    else if (opcode == "GOODBYE")
    {
        // if the input is 'GOODBYE', set the flag to false
        DEBUG_PRINT("Received GOODBYE response, setting flag to true\n");
        *flag_to_set = 1; // set the flag to false
        cout << BOLDYELLOW << data.front() << "Goodbye!" << RESET << endl;
        return; // exit the function
    }
    else if (opcode == "CHECKERS_END")
    {
        // CHECKERS_END response indicates the end of the game
        // If it is sent from server, the data contains won/lost info, so just print it
        // If it is sent from client, the data contains "s" for surrender
        DEBUG_PRINT("Received CHECKERS_END response, setting flag to true\n");
        // If the server receives it, it means the client surrendered
        if (data.front() == "s")
        {
            cout << BOLDYELLOW << "client surrendered, game over!" << RESET << endl;
        }
        else
        {
            cout << BOLDYELLOW << data.front() << RESET << endl;
        }
        return; // exit the function
    }
    else if (opcode == "QUESTION")
    {
        // QUESTION response contains a question and options
        // The first element will be the question, and the rest will be the options
        // All will be printed
        DEBUG_PRINT("Received QUESTION response\n");
        // the first element will be the question
        cout << BOLDYELLOW << "Question: " << data.front() << RESET << endl;
        data.erase(data.begin()); // remove the question from the vector
        // the rest will be the options
        cout << BOLDYELLOW << "Options: " << endl;
        int index = 0; // index for options
        for (auto item : data)
        {
            cout << "   " << index << ". " << item << endl; // print each option
            index++;
        }
        cout << RESET;

        // Prompt the user for an answer
        cout << BOLDYELLOW << "Please enter your answer (index of the option): " << RESET;
        int answer;
        cin >> answer; // read the answer from the user
        // Validate the answer input
        while (answer < 0 || answer >= data.size())
        {
            // if the answer is not valid, prompt the user again
            cerr << ERROR << "Invalid choice. Please choose a valid option." << RESET << endl;
            cout << BOLDYELLOW << "Please choose a game (0 for Checkers): ";
            cin >> answer;
        }
        // Send the answer back to the server
        if (send_to(socket, "ANSWER", to_string(answer)) < 0)
        {
            cerr << ERROR << "Failed to send answer to server" << RESET << endl;
            return;
        }
    }
    else if (opcode == "QUESTION_STR")
    {
        // QUESTION_STR response contains a question as a string
        // This is used for the nickname question
        DEBUG_PRINT("Received QUESTION_STR response\n");
        cout << BOLDYELLOW << "Question: " << data.front() << RESET << endl;

        // Prompt the user for an answer
        string answer;
        while (true)
        {
            cout << BOLDYELLOW << "Please enter your nickname: " << RESET;
            cin >> answer; // read the answer from the user
            if (cin.fail())
            {
                cin.clear();                                         // clear the error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
                cout << ERROR << "Invalid input, please try again.\n";
                continue;
            }
            break;
        }

        // Send the answer back to the server
        if (send_to(socket, "ANSWER", answer) < 0)
        {
            cerr << ERROR << "Failed to send answer to server" << RESET << endl;
            return;
        }
    }
    else if (opcode == "WAITING_END")
    {
        LOBBY_MODE = 0;
    }
    else if (opcode == "TEXT")
    {
        // TEXT response contains a message to display
        DEBUG_PRINT("Processing TEXT response\n");
        for (auto item : data)
        {
            cout << item << endl; // print each item in the data vector
        }
    }
    else
    {
        cerr << ERROR << "Unknown opcode received: " << opcode << RESET << endl;
    }
}

void process_response(const map<string, vector<string>> parsed_response, int *answer)
{
    // Process the parsed response based on the opcode
    string opcode = parsed_response.at("type").front();
    vector<string> data = parsed_response.at("data");
    if (opcode == "ANSWER")
    {
        // ANSWER response contains the answer to a question
        DEBUG_PRINT("Processing ANSWER response\n");
        if (data.size() != 1)
        {
            // if there are multiple answers or no answer, we cannot determine the correct answer
            cerr << ERROR << "Received multiple answers or no answer from server" << RESET << endl;
            cerr << "Answers received: ";
            for (const auto &item : data)
            {
                cerr << item << " ";
            }
            cerr << endl;
            return;
        }
        else
        {
            // if there is only one answer, we can assume it is the correct answer
            DEBUG_PRINT("Answer: " + data.front() + "\n");
            *answer = stoi(data.front()); // convert the first element to an integer answer
        }
    }
    else
    {
        cerr << ERROR << "Unknown opcode received: " << opcode << RESET << endl;
    }
}

void process_response(const map<string, vector<string>> parsed_response, string *answer)
{
    // Process the parsed response based on the opcode
    string opcode = parsed_response.at("type").front();
    vector<string> data = parsed_response.at("data");
    if (opcode == "ANSWER")
    {
        // ANSWER response contains the answer to a question
        DEBUG_PRINT("Processing ANSWER response\n");
        if (data.size() != 1)
        {
            // if there are multiple answers or no answer, we cannot determine the correct answer
            cerr << ERROR << "Received multiple answers or no answer from server" << RESET << endl;
            cerr << "Answers received: ";
            for (const auto &item : data)
            {
                cerr << item << " ";
            }
            cerr << endl;
            return;
        }
        else
        {
            // if there is only one answer, we can assume it is the correct answer
            DEBUG_PRINT("Answer: " + data.front() + "\n");
            *answer = data.front(); // convert the first element to an integer answer
        }
    }
    else
    {
        cerr << ERROR << "Unknown opcode received: " << opcode << RESET << endl;
    }
}

void process_response(const map<string, vector<string>> parsed_response, Session *curr_sess)
{
    // Process the parsed response based on the opcode
    string opcode = parsed_response.at("type").front();
    vector<string> data = parsed_response.at("data");
    if (opcode == "CHECKERS_MOVES")
    {
        // CHEKERS_MOVES response contains the moves made by the player
        DEBUG_PRINT("Processing CHECKERS_MOVES response\n");
        // decode move into move object
        if (data.front() == "q")
        {
            // curr_sess->quit_requested = true; // set the quit requested flag to true
            curr_sess->quit_requested = curr_sess->current_player->get_id();
            DEBUG_PRINT("Received quit request from player, setting quit_requested flag to true\n");
            cout << BOLDYELLOW << "Player requested to quit the game." << RESET << endl;
            return; // exit the function
        }

        Move new_move = decode_move_response(data.front()); // create a new move object from the string
        curr_sess->prev_move = new_move;                    // tell the session about the move
        DEBUG_PRINT("Received move: " + new_move.get_move_info() + "\n");
    }
    else
    {
        cerr << ERROR << "Unknown opcode received: " << opcode << RESET << endl;
    }
}

void get_response(int sock, string *resp)
{
    string response = receive_from(sock);
    if (response.empty())
    {
        throw runtime_error("No response received from server");
    }
    map<string, vector<string>> response_structured = parsed_response(response);
    process_response(response_structured, resp);
}

void get_response(int sock, int *flag = nullptr, int opt = 0)
{
    string response = receive_from(sock);
    if (response.empty())
    {
        throw runtime_error("No response received from server");
    }
    map<string, vector<string>> response_structured = parsed_response(response);
    if (opt == 0)
        process_response(response_structured, sock, flag);
}

void get_response(int sock)
{
    int opt = 0; // to remove
    string response = receive_from(sock);
    if (response.empty())
    {
        throw runtime_error("No response received from server");
    }
    map<string, vector<string>> response_structured = parsed_response(response);
    if (opt == 0)
        process_response(response_structured, sock, nullptr);
}

void get_response(int sock, Session *curr_sess)
{
    string response = receive_from(sock);
    if (response.empty())
    {
        throw runtime_error("No response received from server");
    }
    map<string, vector<string>> response_structured = parsed_response(response);
    process_response(response_structured, curr_sess);
}

string question_to_string(const string question, const vector<string> options)
{
    string question_str = question;
    for (const auto &option : options)
    {
        question_str += ";" + option;
    }
    return question_str;
}

void waiting_message(const string &message)
{
    while (LOBBY_MODE == 1) // wait until the lobby mode is deactivated
    {
        cout << "\r";
        // Display a waiting message in the lobby
        cout << BOLDYELLOW << message << ".";
        usleep(500 * 1000);
        if (LOBBY_MODE == 0) // check if the lobby mode is deactivated
        {
            cout << RESET << endl; // reset the color and exit the loop
            return;
        }
        cout << BOLDYELLOW << "."; // add a dot to the message
        usleep(500 * 1000);
        if (LOBBY_MODE == 0) // check if the lobby mode is deactivated
        {
            cout << RESET << endl; // reset the color and exit the loop
            return;
        }
        cout << BOLDYELLOW << "." << RESET << endl; // add another dot and reset the color
    }
}

void print_banner()
{
    DEBUG_PRINT("------------------ Printing banner -----------------\n");
    string banner = R"(
        ____  ____   ___      _ _____ _  _______   ____  
        |  _ \|  _ \ / _ \    | | ____| |/ /_   _| |___ \ 
        | |_) | |_) | | | |_  | |  _| | ' /  | |     __) |
        |  __/|  _ <| |_| | |_| | |___| . \  | |    / __/ 
        |_|   |_| \_\\___/ \___/|_____|_|\_\ |_|   |_____|
    )";
    cout << BOLDYELLOW << banner << RESET << endl;
}

bool Session::is_full() const
{
    // if AI mode, only player1 is needed to be set
    if (player1 != nullptr && player2 == nullptr && player1->get_chosen_game_mode() == 1)
        return true;
    // if multiplayer mode, both players need to be set
    else if (player1 != nullptr && player2 != nullptr)
        return true;
    else
        return false; // otherwise, the session is not full
}