#include "../request_helpers.hpp"

using namespace std;

int main()
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
    struct sockaddr_in ip4addr;   // destination address
    string dest_ip = "127.0.0.1"; // localhost
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

    // perform handshake with server
    DEBUG_PRINT("Performing handshake with server\n");
    if (handshake(sock) < 0)
    {
        cerr << ERROR << "Handshake failed" << RESET << endl;
        close(sock);
        return -1;
    }
    // close socket
    close(sock);
    return 0;
}