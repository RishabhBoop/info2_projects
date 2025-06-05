#include "request_helpers.hpp"

using namespace std;

int send_to(int sock, const string opcode, const string message)
{
    string tosend = opcode + ":<" + message + ">" + CONTROL_CHAR;
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

bool opcode_is_valid(const string &opcode)
{
    return opcode == "HEY" || opcode == "PERFECT" || opcode == "TEXT" || opcode == "OK" || opcode == "MENU" || opcode == "GAME" || opcode == "GOODBYE";
}

bool opcode_is_valid(const string &opcode, const string type)
{
    if (type == "empty headers")
        return opcode == "HEY" || opcode == "PERFECT" || opcode == "OK" || opcode == "GOODBYE";
    else
        return opcode == "HEY" || opcode == "PERFECT" || opcode == "TEXT" || opcode == "OK" || opcode == "MENU" || opcode == "GAME" || opcode == "GOODBYE";
}

map<string, list<string>> parsed_response(string response)
{
    list<string> data_list; // list to hold data
    list<string> type_list; // list to hold type (can be empty or have a value)
    map<string, list<string>> parsed_map = {{"type", type_list}, {"data", data_list}};

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
    bool opcode_type_empty_header = opcode_is_valid(part, "empty headers");

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
        parsed_map["data"].clear(); // clear the data list
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
                parsed_map["data"].push_back(item); // add the item to the data list
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