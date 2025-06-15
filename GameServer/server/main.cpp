#include "../request_helpers.hpp"

using namespace std;

void thread_function(int);

vector<int> NUM_CONNECTIONS; // global variable to keep track of the number of connections

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
    server_service.sin_addr.s_addr = INADDR_ANY; // allow any ipv4 address
    server_service.sin_family = AF_INET;         // IPv4
    server_service.sin_port = htons(PORT);       // listen on port SERVER_PORT
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

    DEBUG_PRINT("----------------- Server is ready to accept connections -----------------\n");

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
    NUM_CONNECTIONS.push_back(client_sock); // increment the number of connections
    DEBUG_PRINT("Accepted connection from client\n");

    thread t1(thread_function, client_sock); // create a new thread to handle the client connection

    // close sockets
    t1.join(); // wait for the thread to finish
    cout << INFO << "Client thread finished" << RESET << endl;
    close(sock);
    cout << INFO << "Server socket closed" << RESET << endl;

    return 0;
}



void thread_function(int client_sock)
{
    /* ---------------- Handshake -------------------------- */
    DEBUG_PRINT("------------------ Handshake with client -----------------\n");
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
    /* ---------------- End Handshake -------------------------- */

    /* ---------------- send banner --------------------------*/
    DEBUG_PRINT("------------------ Sending banner to client -----------------\n");
    string banner = R"(
        ____  ____   ___      _ _____ _  _______   ____  
        |  _ \|  _ \ / _ \    | | ____| |/ /_   _| |___ \ 
        | |_) | |_) | | | |_  | |  _| | ' /  | |     __) |
        |  __/|  _ <| |_| | |_| | |___| . \  | |    / __/ 
        |_|   |_| \_\\___/ \___/|_____|_|\_\ |_|   |_____|
    )";
    if (send_to(client_sock, "TEXT", banner) < 0)
    {
        cerr << ERROR << "Error sending response back to client: " << strerror(errno) << RESET << endl;
        close(client_sock);
        return;
    }
    // the client should have sent an "OK" response after receiving the banner
    response = receive_from(client_sock); // receive data from the client again (should be "OK")
    if (parsed_response(response)["type"].front() == "OK")
    {
        // DEBUG_PRINT("Client confirmed banner successfully\n");
        cout << SUCCESS << "Client confirmed banner successfully" << RESET << endl;
    }
    else
    {
        cerr << ERROR << "Client did not confirm banner successfully: " << response << RESET << endl;
    }
    /* ---------------- End send banner --------------------------*/

    /* ---------------- Send Question which game --------------------------*/
    DEBUG_PRINT("------------------ Asking client which game to play -----------------\n");
    vector<string> options = {"Checkers"};
    string question = question_to_string("Which game do you want to play?", options);
    if (send_to(client_sock, "QUESTION", question) < 0)
    {
        cerr << ERROR << "Error sending question to client: " << strerror(errno) << RESET << endl;
        close(client_sock);
        return;
    }
    // receive the answer from the client
    response = receive_from(client_sock); // receive data from the client again (should be "ANSWER")
    int chosen_answer = -1;
    if (parsed_response(response)["type"].front() == "ANSWER")
    {
        // DEBUG_PRINT("Client answered the question successfully\n");
        cout << SUCCESS << "Client answered the question successfully" << RESET << endl;
        // process the answer
        int* ptr_chosen_answer = &chosen_answer;
        process_response(parsed_response(response), ptr_chosen_answer);
        cout << INFO << "Client chose option: " << chosen_answer << RESET << endl;
    }
    else
    {
        cerr << ERROR << "Client did not answer the question successfully: " << response << RESET << endl;
    }
    /* ---------------- End Send Question which game --------------------------*/

    /* ---------------- Send number of players --------------------------*/
    DEBUG_PRINT("------------------ Sending number of players to client -----------------\n");
    string num_players;
    if (NUM_CONNECTIONS.size() == 1)
    {
        num_players = "you are alone"; // if this is the first connection, the client is alone
    }
    else
    {
        num_players = "There are " + to_string(NUM_CONNECTIONS.size()-1) + "other players"; // otherwise, send the number of connections
    }
    if (send_to(client_sock, "TEXT", num_players) < 0)
    {
        cerr << ERROR << "Error sending number of players to client: " << strerror(errno) << RESET << endl;
        close(client_sock);
        return;
    }
    // the client should have sent an "OK" response after receiving the number of players
    response = receive_from(client_sock); // receive data from the client again (should be "OK")
    if (parsed_response(response)["type"].front() == "OK")
    {
        // DEBUG_PRINT("Client confirmed number of players successfully\n");
        cout << SUCCESS << "Client confirmed number of players successfully" << RESET << endl;
    }
    else
    {
        cerr << ERROR << "Client did not confirm number of players successfully: " << response << RESET << endl;
    }
    /* ---------------- End Send number of players --------------------------*/


    close(client_sock);
    cout << INFO << "Client socket closed" << RESET << endl;
    
    return;
}
