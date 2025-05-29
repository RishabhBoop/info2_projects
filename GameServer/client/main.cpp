#include "../request_helpers.hpp"

using namespace std;

int main()
{
    cout << "Client started" << endl;
    // init socket
    int sock = socket(AF_INET, SOCK_STREAM, 0); // create a TCP socket for connecting to a server
    if (sock < 0)
    {
        cerr << "Error creating socket: " << strerror(errno) << endl;
        return -1;
    }
    printf("Socket created successfully\n");

    // define target address
    struct sockaddr_in ip4addr;   // destination address
    string dest_ip = "127.0.0.1"; // localhost
    inet_pton(AF_INET, dest_ip.c_str(), &ip4addr.sin_addr);
    cout << "Target address set: " << dest_ip << endl;

    // set up socket address
    struct sockaddr_in clientService;          // client address
    clientService.sin_family = AF_INET;        // IPv4
    clientService.sin_port = htons(PORT);      // port number
    clientService.sin_addr = ip4addr.sin_addr; // bind socket to the target address
    cout << "Socket address set: " << dest_ip << ":" << PORT << endl;

    // connect to server
    int iResult = connect(sock, (struct sockaddr *)&clientService, sizeof(clientService));
    if (iResult < 0)
    {
        cerr << "Error connecting to server: " << strerror(errno) << endl;
        close(sock);
        return -1;
    }
    printf("Connected to server\n");

    // perform handshake with server
    cout << "Performing handshake with server..." << endl;
    if ( handshake(sock) < 0 )
    {
        cerr << "Handshake failed" << endl;
        close(sock);
        return -1;
    }
    printf("Handshake successful\n");
    // close socket
    close(sock);
    return 0;
}