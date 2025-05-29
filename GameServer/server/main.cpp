#include "request_helpers.hpp"

using namespace std;

int main()
{
    /* init socket
    * This creates a TCP socket that the server will use to listen for incoming connections.    
    */
    int sock = socket(AF_INET, SOCK_STREAM, 0); // create a TCP socket for listening for incoming connections
    if (sock < 0)
    {
        cerr << "Error creating socket: " << strerror(errno) << endl;
        return -1;
    }
    printf("Socket created successfully\n");

    /* specify connection
    * It is the connection (ports, addresses, etc.) that the server will use to accept incoming connections.
    */
    struct sockaddr_in server_service;            
    server_service.sin_addr.s_addr = INADDR_ANY;  // allow any ipv4 address
    server_service.sin_family = AF_INET;          // IPv4
    server_service.sin_port = htons(SERVER_PORT); // listen on port SERVER_PORT
    printf("Connection specified: %s:%d\n", inet_ntoa(server_service.sin_addr), ntohs(server_service.sin_port));

    /* Bind socket to port
    * This binds the socket to the specified service.
    * It allows the server to listen for incoming connections on that port.
    */
    int res = bind(sock, (struct sockaddr *)&server_service, sizeof(server_service));
    if (res < 0)
    {
        cerr << "Error binding socket: " << strerror(errno) << endl;
        close(sock);
        return -1;
    }
    printf("Socket bound to port %d\n", SERVER_PORT);

    /* listen for incoming connections
    * This tells the socket to listen for incoming connections.
    */
    res = listen(sock, SOMAXCONN);
    if (res < 0)
    {
        cerr << "Error listening on socket: " << strerror(errno) << endl;
        close(sock);
        return -1;
    }
    printf("Listening for incoming connections...\n");

    /* Accept connection
    * If a client tries to connect, the server will accept the connection and create a new socket for communication with that client.
    */
    int client_sock = accept(sock, nullptr, nullptr);
    if (client_sock < 0)
    {
        cerr << "Error accepting connection: " << strerror(errno) << endl;
        close(sock);
        return -1;
    }
    printf("Accepted connection from client\n");

    /* ---------------------------------------------------- */
    string response = receive_from_client(client_sock); // receive data from the client
    cout << "Received from client: " << response << endl;


    // close socket
    close(sock);
    close(client_sock);
    printf("Socket closed\n");

    return 0;
}