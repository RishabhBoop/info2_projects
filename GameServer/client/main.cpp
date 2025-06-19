#include "../request_helpers.hpp"

using namespace std;

int main()
{
#pragma region Send Banner
    DEBUG_PRINT("------------------ Printing banner -----------------\n");
    string banner = R"(
        ____  ____   ___      _ _____ _  _______   ____  
        |  _ \|  _ \ / _ \    | | ____| |/ /_   _| |___ \ 
        | |_) | |_) | | | |_  | |  _| | ' /  | |     __) |
        |  __/|  _ <| |_| | |_| | |___| . \  | |    / __/ 
        |_|   |_| \_\\___/ \___/|_____|_|\_\ |_|   |_____|
    )";
    cout << BOLDYELLOW << banner << RESET << endl;
#pragma endregion

#pragma region Init
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
#pragma endregion

    int goodbye_received = 0;                      // flag to check if goodbye message is received
    int *ptr_goodbye_received = &goodbye_received; // pointer to the goodbye_received flag

#pragma region Handshake
    // perform handshake with server
    DEBUG_PRINT("Performing handshake with server\n");
    if (handshake(sock) < 0)
    {
        cerr << ERROR << "Handshake failed" << RESET << endl;
        close(sock);
        return -1;
    }
#pragma endregion

#pragma region Nickname
    // receive question: nickname from server
    try
    {
        get_response(sock, 1); // 1 means we expect a question response
    }
    catch (const runtime_error &e)
    {
        cerr << ERROR << "Runtime error: " << e.what() << RESET << endl;
    }
    catch (const invalid_argument &e)
    {
        cerr << ERROR << "Invalid argument: " << e.what() << RESET << endl;
    }
    catch (...)
    {
        cerr << ERROR << "An unexpected error occurred" << RESET << endl;
    }
    DEBUG_PRINT("Received and processed question from server\n");
#pragma endregion

#pragma region Game Choice
    // Receive a question of choosing games from server
    try
    {
        get_response(sock, 1); // 1 means we expect a question response
    }
    catch (const runtime_error &e)
    {
        cerr << ERROR << "Runtime error: " << e.what() << RESET << endl;
    }
    catch (const invalid_argument &e)
    {
        cerr << ERROR << "Invalid argument: " << e.what() << RESET << endl;
    }
    catch (...)
    {
        cerr << ERROR << "An unexpected error occurred" << RESET << endl;
    }
    DEBUG_PRINT("Received and processed game choice question from server\n");
#pragma endregion

#pragma region Game Mode
    // receive question of game mode from server
    try
    {
        get_response(sock, 1); // 1 means we expect a question response
    }
    catch (const runtime_error &e)
    {
        cerr << ERROR << "Runtime error: " << e.what() << RESET << endl;
    }
    catch (const invalid_argument &e)
    {
        cerr << ERROR << "Invalid argument: " << e.what() << RESET << endl;
    }
    catch (...)
    {
        cerr << ERROR << "An unexpected error occurred" << RESET << endl;
    }
    DEBUG_PRINT("Received and processed game mode question from server\n");
#pragma endregion

#pragma region Waiting for Players
    thread waiting_thread(waiting_message, "Waiting for other players..."); // 1 means we expect a question/lobby message
    while (LOBBY_MODE == 1)                                                 // wait for a maximum of 10 seconds
    {
        try
        {
            get_response(sock, 0);
        }
        catch (const runtime_error &e)
        {
            // cerr << ERROR << "Runtime error: " << e.what() << RESET << endl;
            continue;
        }
        catch (const invalid_argument &e)
        {
            // cerr << ERROR << "Invalid argument: " << e.what() << RESET << endl;
            continue;
        }
        catch (...)
        {
            // cerr << ERROR << "An unexpected error occurred" << RESET << endl;
            continue;
        }
    }

    DEBUG_PRINT("Received and processed waiting message from server, waiting for waiting_thread to finish\n");
    waiting_thread.join(); // wait for the waiting thread to finish
    DEBUG_PRINT("Waiting thread finished\n");
#pragma endregion

#pragma region Player ID
    DEBUG_PRINT("Receiving player ID from server\n");
    // receive player ID from server
    try
    {
        get_response(sock, 0); // 0 means we expect a text response
    }
    catch (const runtime_error &e)
    {
        cerr << ERROR << "Runtime error: " << e.what() << RESET << endl;
    }
    catch (const invalid_argument &e)
    {
        cerr << ERROR << "Invalid argument: " << e.what() << RESET << endl;
    }
    catch (...)
    {
        cerr << ERROR << "An unexpected error occurred" << RESET << endl;
    }
    DEBUG_PRINT("Received and processed player ID from server\n");
#pragma endregion

#pragma region Game Logic
    while (goodbye_received == 0)
    {
        // receive gamestate from server, process it and send back the move
        try
        {
            DEBUG_PRINT("Trying to receive game state from server\n");
            get_response(sock, ptr_goodbye_received, 0);
        }
        catch (const runtime_error &e)
        {
            cerr << ERROR << "Runtime error: " << e.what() << RESET << endl;
        }
        catch (const invalid_argument &e)
        {
            cerr << ERROR << "Invalid argument: " << e.what() << RESET << endl;
        }
        catch (...)
        {
            cerr << ERROR << "An unexpected error occurred" << RESET << endl;
        }
    }
#pragma endregion

    // close socket
    close(sock);
    return 0;
}