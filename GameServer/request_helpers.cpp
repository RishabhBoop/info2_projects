#include "request_helpers.hpp"

using namespace std;

int send_to(int sock, const string message)
{
    string tosend = message + "\a";
    // send message to client
    ssize_t bytes_sent = send(sock, tosend.c_str(), tosend.length(), 0);
    if (bytes_sent < 0)
    {
        cerr << "Error sending message to server: " << strerror(errno) << endl;
        return -1;
    }
    return 0; // success
}

string receive_from(int sock)
{
    printf("Waiting for server message...\n");
    int res = 0; // initialize res to 0
    string received_data = ""; // string to hold received data
    bool cc_found = false; // flag to check if control character is found
    while (cc_found == false) // continue until control character is found
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

int handshake(int &sock)
{
    // send a handshake message to the server
    string handshake_message = "Hello, server!";
    cout << "Sending handshake message to server: " << handshake_message << endl;
    if (send_to(sock, handshake_message) < 0)
    {
        cerr << "Failed to send handshake message to server" << endl;
        return -1;
    }
    cout << "Handshake message sent successfully\n";

    // receive a response from the server
    string response = receive_from(sock);
    cout << "Received response from server: " << response << endl;

    if(handshake_message != parse_response(response))
    {
        cerr << "Handshake failed: response does not match handshake message" << endl;
        return -1;
    }
    return 0; // success
}