#include "request_helpers.hpp"

using namespace std;

int send_to_client(int client_sock, const string message)
{
    // send message to client
    ssize_t bytes_sent = send(client_sock, message.c_str(), message.length(), 0);
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
    int res = 0; // initialize res to 0
    string received_data = ""; // string to hold received data
    do
    {
        char buffer[BUFFER_SIZE];
        res = recv(client_sock, &buffer, BUFFER_SIZE, 0); // receive data
        if (res > 0)
        {
            // successful in receiving data
            for (int i = 0; i < res; i++)
            {
                // concat to string
                received_data += buffer[i];
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
    } while (res > 0);
    return received_data;
}