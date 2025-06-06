#include "request_helpers.hpp"

using namespace std;

int send_to_client(int client_sock, const string message)
{
    string tosend = message + "\a"; // append control characters to the message
    // send message to client
    ssize_t bytes_sent = send(client_sock, tosend.c_str(), tosend.length(), 0);
    if (bytes_sent < 0)
    {
        cerr << "Error sending message to client: " << strerror(errno) << endl;
        return -1;
    }
    return 0; // success
}

string receive_from_client(int client_sock)
{
    printf("Waiting for client message...\n");
    string received_data = ""; // string to hold received data
    bool cc_found = false; // flag to check if control character is found
    while (cc_found == false) // continue until control character is found
    {
        char buffer[BUFFER_SIZE];
        int res = recv(client_sock, &buffer, BUFFER_SIZE, 0); // receive data
        if (res > 0)
        {
            // successful in receiving data
            for (int i = 0; i < res; i++)
            {
                // concat to string
                received_data += buffer[i];
                if (buffer[i] == '\a') // check for control character
                {
                    // stop receiving data when control character is found
                    cout << "Control character found, stopping reception." << endl;
                    cc_found = true; // set flag to true
                    break;
                }
            }
        }
        else if (res < 0)
        {
            // e.g. -1: Error
            cerr << "Error receiving data: " << strerror(errno) << endl;
            throw runtime_error("Error receiving data from client");
        }
        else
        {
            // no more data: done
            break;
        }
    }
    cout << "raw received data: " << received_data << endl;

    return received_data;
}

string parse_response(const string &response)
{
    // remove the trailing control characters
    size_t pos = response.find("\a");
    if (pos == string::npos)
    {
        throw runtime_error("No control characters found in received data");
    }
    string parsed_response = response.substr(0, pos); // truncate the string at the first control character
    printf("Parsed response: %s\n", parsed_response.c_str());
    return parsed_response;
}