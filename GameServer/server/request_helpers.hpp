#ifndef REQUEST_HELPERS_HPP
#define REQUEST_HELPERS_HPP

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring> // For strerror
#include <unistd.h>
#include <locale> // For locale (needed by wcout)

#define BUFFER_SIZE 1024
#define SERVER_PORT 26000
#define TIMEOUT 10

using namespace std;

int send_to_client(int, const string);

string receive_from_client(int);

string parse_response(const string &response);


#endif