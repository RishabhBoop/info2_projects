#include "../request_helpers.hpp"

using namespace std;

int main()
{
    int dummy_flag = 0;           // for the get_response, if you do not need to set any flags
    int *dummy_ptr = &dummy_flag; // pointer to a dummy flag, used for get_response function
    print_banner();               // print the banner for welcome
#pragma region Init Client Socket
    int sock = init_client_sock();
    if (sock < 0)
    {
        return sock; // exit if socket initialization failed
    }
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
    try
    {
        get_response(sock);
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
        get_response(sock);
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
        get_response(sock);
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
    int* ptr_lobby_mode = &LOBBY_MODE; // pointer to the lobby mode variable
    thread waiting_thread(waiting_message, "Waiting for other players", BOLDYELLOW, ptr_lobby_mode);
    while (LOBBY_MODE == 1)
    {
        try
        {
            get_response(sock);
        }
        catch (const runtime_error &e)
        {
            cerr << ERROR << "Runtime error: " << e.what() << RESET << endl;
            continue;
        }
        catch (const invalid_argument &e)
        {
            cerr << ERROR << "Invalid argument: " << e.what() << RESET << endl;
            continue;
        }
        catch (...)
        {
            cerr << ERROR << "An unexpected error occurred" << RESET << endl;
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
        get_response(sock);
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

    int times_tried = 0;
#pragma region Game Logic
    while (goodbye_received == 0 && times_tried < 10)
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
            times_tried++;
        }
        catch (const invalid_argument &e)
        {
            cerr << ERROR << "Invalid argument: " << e.what() << RESET << endl;
            times_tried++;
        }
        catch (...)
        {
            cerr << ERROR << "An unexpected error occurred" << RESET << endl;
            times_tried++;
        }
    }
#pragma endregion

    // close socket
    close(sock);
    return 0;
}