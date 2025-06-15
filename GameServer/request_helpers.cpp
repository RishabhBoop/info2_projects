#include "request_helpers.hpp"

using namespace std;

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
                    DEBUG_PRINT("Control character found in received data\n");
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
    return opcode == "HEY" || opcode == "PERFECT" || opcode == "TEXT" || opcode == "OK" || opcode == "QUESTION" || opcode == "ANSWER" || opcode == "GOODBYE";
}

bool opcode_is_valid(const string opcode, const string type)
{
    if (type == "empty data allowed")
        return opcode == "HEY" || opcode == "PERFECT" || opcode == "OK" || opcode == "GOODBYE";
    else
        return opcode == "HEY" || opcode == "PERFECT" || opcode == "TEXT" || opcode == "OK" || opcode == "QUESTION" || opcode == "ANSWER" || opcode == "GOODBYE";
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
    DEBUG_PRINT("Parsed opcode: " + part + "\n");
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
        DEBUG_PRINT("Parsing data part: " + data_part + "\n");

        // split the data part by semicolon (;) to get the individual data items
        size_t start = 0;
        size_t end = data_part.find(';');
        while (end != string::npos)
        {
            string item = data_part.substr(start, end - start);
            if (!item.empty())
            {
                parsed_map["data"].push_back(item); // add the item to the data vector
                DEBUG_PRINT("Parsed data item: " + item + "\n");
            }
            start = end + 1;
            end = data_part.find(';', start);
        }

        // add the last item (if any)
        if (start < data_part.length())
        {
            parsed_map["data"].push_back(data_part.substr(start));
            DEBUG_PRINT("Parsed last data item: " + data_part.substr(start) + "\n");
        }
    }
    else
    {
        throw runtime_error("Invalid response format: no data found");
    }

    return parsed_map;
}

void process_response(const map<string, vector<string>> parsed_response)
{
    // Process the parsed response based on the opcode
    string opcode = parsed_response.at("type").front();
    vector<string> data = parsed_response.at("data");
    if (opcode == "TEXT")
    {
        // TEXT response contains a message to display
        DEBUG_PRINT("Processing TEXT response\n");
        for (auto item : data)
        {
            cout << item << endl; // print each item in the data vector
        }
    }
    else if (opcode == "OK")
    {
        // OK response indicates that the previous request was successful
        // Do nothing
        DEBUG_PRINT("Received OK response\n");
    }
    else if (opcode == "QUESTION")
    {
        // The question response needs another (overloaded) function to process it
        DEBUG_PRINT("Received QUESTION response, redirecting to process_response with socket\n");
        throw invalid_argument("No socket provided for QUESTION response processing");
    }
    else if (opcode == "ANSWER")
    {
        // The answer response needs another (overloaded) function to process it
        DEBUG_PRINT("Received ANSWER response, redirecting to process_response with answer\n");
        throw invalid_argument("No answer int provided for ANSWER response processing");
    }
    else if (opcode == "CHECKERS_BOARD_TURN")
    {
        // The checkers board turn response needs another (overloaded) function to process it
        DEBUG_PRINT("Received CHECKERS_BOARD_TURN response, redirecting to process_response with moves\n");
        throw invalid_argument("No moves list provided for CHECKERS_BOARD_TURN response processing");
    }
    else if (opcode == "HEY")
    {
        DEBUG_PRINT("Received HEY response, performing handshake\n");
    }
    else if (opcode == "PERFECT")
    {
        DEBUG_PRINT("Received PERFECT response, handshake successful\n");
    }
    else if (opcode == "GOODBYE")
    {
        DEBUG_PRINT("Received GOODBYE response, closing connection\n");
    }
    else
    {
        cerr << ERROR << "Unknown opcode received: " << opcode << RESET << endl;
    }
}

void process_response(const map<string, vector<string>> parsed_response, int socket)
{
    // Process the parsed response based on the opcode
    string opcode = parsed_response.at("type").front();
    vector<string> data = parsed_response.at("data");
    if (opcode == "QUESTION")
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
        for (auto item : data)
        {
            cout << "   - " << item << endl; // print each option
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
            cout << INFO << "Please choose a game (0 for Checkers): ";
            cin >> answer;
        }
        // Send the answer back to the server
        if (send_to(socket, "ANSWER", to_string(answer)) < 0)
        {
            cerr << ERROR << "Failed to send answer to server" << RESET << endl;
            return;
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
            cout << BOLDYELLOW << "Answer: " << data.front() << RESET << endl;
            *answer = stoi(data.front()); // convert the first element to an integer answer
        }
    }
    else
    {
        cerr << ERROR << "Unknown opcode received: " << opcode << RESET << endl;
    }
}

void process_response(const map<string, vector<string>> parsed_response, vector<int> *moves)
{
    // Process the parsed response based on the opcode
    string opcode = parsed_response.at("type").front();
    vector<string> data = parsed_response.at("data");

    // FORMAT: CHECKERS_BOARD_TURN:<x_og;y_og;x_new;y_new;>
    if (opcode == "CHECKERS_BOARD_TURN")
    {
        DEBUG_PRINT("Processing CHECKERS_BOARD_TURN response\n");
        moves->clear();
        for (const auto item : data)
        {
            moves->push_back(stoi(item)); // convert each item to an integer and add to the moves vector
        }
    }
    else
    {
        cerr << ERROR << "Unknown opcode received: " << opcode << RESET << endl;
    }
}

string question_to_string(const string question, const vector<string> options)
{
    string question_str = question;
    for (const auto &option : options)
    {
        question_str += ";" + option;
    }
    DEBUG_PRINT("Formatted question string: " + question_str + "\n");
    return question_str;
}

