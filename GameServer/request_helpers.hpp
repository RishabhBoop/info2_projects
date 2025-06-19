#ifndef REQUEST_HELPERS_HPP
#define REQUEST_HELPERS_HPP

#include "includes.hpp"
#include "./server/classes.hpp"
#include "./server/mcts_algorithm.hpp"

using namespace std;

extern int LOBBY_MODE;        // Lobby mode for client; 0 = not actived, 1 = activated
extern std::string LOBBY_TXT; // Text to be displayed in the lobby, will be sent by server
// forward declaration
class Player;
class Session;

inline void clear_screen();

void waiting_message(const string &);

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
 *        No data is sent, only the opcode.
 * @param sockfd The socket file descriptor.
 * @param message The message string to send.
 * @return 0 on success, -1 on failure.
 */
int send_to(int, const string);

/**
 * @brief Receives the raw message from a socket.
 * @param sockfd The socket file descriptor.
 * @return The received message as a string.
 */
string receive_from(int);

/**
 * @brief Client: Performs a handshake with a server.
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
 * @brief Processes a parsed TEXT response.
 *        Use this overload when expecting TEXT or WAITING_END.
 * @param response The parsed response map.
 */
void process_response(const map<string, vector<string>>);

/**
 * @brief Processes a parsed QUESTION response, interacts with the user to collect an answer and sends the answer back.
 *        Use this overload when you are expecting a question (QUESTION/QUESTION_STR) or WAITING_END.
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
 * @brief Processes a parsed ANSWER response and stores the extracted string value in the provided string pointer.
 *        Use this overload when you want to extract a string value from the response.
 * @param response The parsed response map.
 * @param value Pointer to a string where the extracted value will be stored.
 */
void process_response(const map<string, vector<string>>, string *);

/**
 * @brief Processes a parsed response and sets a boolean flag based on the response type.
 *        Use this overload when you want to extract a boolean value from the response data.
 *        This is used for GOODBYE.
 * @param response The parsed response map.
 * @param value Pointer to the boolean where the extracted value will be stored.
 */
void process_response(const map<string, vector<string>>, bool *);

/**
 * @brief Processes a parsed response and updates the current session state based on the response.
 *        Use this overload when you want to update the session state based on the response data.
 *        This is used for CHECKERS_MOVES
 * @param parsed_response
 * @param curr_sess
 */
void process_response(const map<string, vector<string>> parsed_response, Session *curr_sess);

/**
 * @brief Processes a parsed response and updates the game state and a flag based on the response data.
 *        This function is used to handle responses that may require a flag to be set, such as GOODBYE messages.
 *        This is used for CHECKERS_STATE, GOOOBYE and CHECKERS_END
 *
 * @param parsed_response
 * @param socket
 * @param flag_to_set
 */
void process_response(const map<string, vector<string>> parsed_response, int socket, int *flag_to_set);

/**
 * @brief Receives a response and processes it.
 *        Used for ANSWER (string) responses.
 * @param sock The socket file descriptor.
 * @param resp Pointer to a string where the extracted value will be stored.
 */
void get_response(int, string *);

/**
 * @brief Receives a response and processes it.
 *        Used for CHECKERS_STATE, CHECKERS_MOVES, GOODBYE responses.
 * 
 * @note This function is e.g. in game logic. 
 * @param sock The socket file descriptor.
 * @param flag Pointer to an integer where the response will be stored.
 * @param opt An integer option to specify the type of response handling (it can be anything)
 */
void get_response(int, int *, int);

/**
 * @brief Receives a response and processes it.
 *       Use this overload when you want to handle a response that does not contain any actions or contains a question.
 * @example
 *         get_response(sock); // defaults to 0 means we expect a TEXT response
 *         get_response(sock, 1); // 1 means we expect a QUESTION/WAITING_END message
 * @param sock The socket file descriptor.
 * @param option An integer option to specify the type of response handling (0 means we expect a text response, 1 means we expect a question/lobby message).
 */
void get_response(int, int);


/**
 * @brief Receives a response and processes it.
 *        Used for CHECKERS_MOVES responses.
 * @param sock The socket file descriptor.
 * @param curr_sess Pointer to the current session object.
 */
void get_response(int sock, Session *curr_sess);

/**
 * @brief Converts a question and its options to a string.
 * @param question The question string.
 * @param options The vector of option strings.
 * @return The formatted question string.
 */
string question_to_string(const string, const vector<string>);

GameState decode_state_response(string data);
Move decode_move_response(string data);



void print_banner();

class Player
{
protected:
    int socket;
    string nickname;
    int chosen_game;
    int chosen_game_mode; // 0 = multiplayer, 1 = AI
    Session *session;     // pointer to the session the player is in
    int id;

public:
    // constructor
    Player(int sock = 0, string nickname = "", int chosen_game = 0, int chosen_game_mode = 0, Session *session = nullptr, int id = 0) : socket(sock), nickname(nickname), chosen_game(chosen_game), chosen_game_mode(chosen_game_mode), session(session), id(id) {};

    // setters
    void set_socket(int sock) { socket = sock; }
    void set_nickname(const string spitzname) { nickname = spitzname; }
    void set_chosen_game(int choice) { chosen_game = choice; }
    void set_chosen_game_mode(int choice) { chosen_game_mode = choice; }
    void set_session(Session *sess) { session = sess; }
    void set_id(int player_id) { id = player_id; }

    // getters
    int get_socket() const { return socket; }
    string get_nickname() const { return nickname; }
    int get_chosen_game() const { return chosen_game; }
    int get_chosen_game_mode() const { return chosen_game_mode; }
    Session *get_session() const { return session; }
    int get_id() const { return id; }
};

class Session
{
public:
    Player *player1;
    Player *player2;
    Player *current_player; // pointer to the current player
    Move prev_move;
    GameState curr_state = GameState(Board(), PLAYER1); // current game state, initialized with an empty board and player 1 as the current player
    int first_player = 1;                               // pointer to the first player in the session (by default player1)
    int quit_requested = false;                         // flag to indicate if the player requested to quit
    Session(Player *p1 = nullptr, Player *p2 = nullptr) : player1(p1), player2(p2), current_player(p1) {};

    bool is_full() const; // check if the session is full
};
#endif