#include "../request_helpers.hpp"

using namespace std;

int main()
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
    DEBUG_PRINT("Socket created successfully\n");

    /* specify connection
     * It is the connection (ports, addresses, etc.) that the server will use to accept incoming connections.
     */
    struct sockaddr_in server_service;
    server_service.sin_addr.s_addr = INADDR_ANY;  // allow any ipv4 address
    server_service.sin_family = AF_INET;          // IPv4
    server_service.sin_port = htons(PORT); // listen on port SERVER_PORT
    // printf("Connection specified: %s:%d\n", inet_ntoa(server_service.sin_addr), ntohs(server_service.sin_port));
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
    DEBUG_PRINT("Socket bound to port " + to_string(PORT) + "\n");

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
    DEBUG_PRINT("Accepted connection from client\n");

    /* ---------------- Handshake -------------------------- */
    string response = receive_from(client_sock); // receive data from the client
    cout << INFO << "Received from client: " << response << RESET << endl;
    // echo response back to client (unblock client recv)
    cout << INFO << "Echoing response back to client..." << RESET << endl;
    if (send_to(client_sock, response) < 0)
    {
        cerr << ERROR << "Error sending response back to client: " << strerror(errno) << RESET << endl;
    }
    cout << INFO << "Response sent back to client" << RESET << endl;
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

    // /* ---------------- send banner  --------------------------*/
    string banner = "Welcome to the Game Server!\n";

    
    // close socket
    close(sock);
    close(client_sock);
    printf("Socket closed\n");

    return 0;
}