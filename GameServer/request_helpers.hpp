#ifndef REQUEST_HELPERS_HPP
#define REQUEST_HELPERS_HPP

#include "includes.hpp"

using namespace std;

/**
 * @brief Sends a message with an opcode and data to a socket.
 * @param sockfd The socket file descriptor.
 * @param opcode The opcode string.
 * @param data The data string to send.
 * @return 0 on success, -1 on failure.
 */
int send_to(int, const string, const string);

/**
 * @brief Sends a message to a socket.
 * @param sockfd The socket file descriptor.
 * @param message The message string to send.
 * @return 0 on success, -1 on failure.
 */
int send_to(int, const string);

/**
 * @brief Receives a message from a socket.
 * @param sockfd The socket file descriptor.
 * @return The received message as a string.
 */
string receive_from(int);

/**
 * @brief Performs a handshake with a client or server.
 * @param sockfd Reference to the socket file descriptor.
 * @return 0 on success, -1 on failure.
 */
int handshake(int &);

/**
 * @brief Checks if an opcode is valid.
 * @param opcode The opcode string to check.
 * @return true if valid, false otherwise.
 */
bool opcode_is_valid(const string);

/**
 * @brief Checks if an opcode and sub-opcode are valid.
 * @param opcode The opcode string to check.
 * @param subopcode The sub-opcode string to check.
 * @return true if valid, false otherwise.
 */
bool opcode_is_valid(const string, const string);

/**
 * @brief Parses a response string into a map of opcode and associated data.
 * @param response The response string to parse.
 * @return A map from opcode to a vector of strings containing the data.
 */
map<string, vector<string>> parsed_response(string);

/**
 * @brief Processes a parsed response.
 *        Use this overload when expecting a text response (it will print it).
 * @param response The parsed response map.
 */
void process_response(const map<string, vector<string>>);

/**
 * @brief Processes a parsed QUESTION response and interacts with the user to collect an answer.
 *        Use this overload when you are expecting a question.
 * @param response The parsed response map.
 * @param value The socket file descriptor to send the answer.
 */
void process_response(const map<string, vector<string>>, int);

/**
 * @brief Processes a parsed ANSWER response and stores the answer in the provided integer pointer.
 *        Use this overload when you want to extract the answer value from the response.
 * @param response The parsed response map.
 * @param value Pointer to an integer where the answer will be stored.
 */
void process_response(const map<string, vector<string>>, int *);

/**
 * @brief Processes a parsed CHECKERS_BOARD_TURN response and fills the provided vector with move data.
 *        Use this overload when you want to extract a list of moves from the response.
 * @param response The parsed response map.
 * @param values Pointer to a vector of integers to be filled with move data.
 */
void process_response(const map<string, vector<string>>, vector<int> *);

/**
 * @brief Converts a question and its options to a string.
 * @param question The question string.
 * @param options The vector of option strings.
 * @return The formatted question string.
 */
string question_to_string(const string, const vector<string>);

class Game
{
public:
    int player1_socket;
    int player2_socket;
    int current_player; // 1 for player1, 2 for player2

    
};

#endif