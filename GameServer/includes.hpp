#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring> // For strerror
#include <unistd.h>
#include <locale> // For locale (needed by wcout)


#include <array>
#include <math.h>
#include <fstream>

#include <thread>
#include <mutex>

#include <map>
#include <vector>

/*
https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
https://en.wikipedia.org/wiki/ANSI_escape_code#Colors:~:text=bracketed%20paste%20mode.-,Select%20Graphic%20Rendition%20parameters,-%5Bedit%5D
https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
*/

#ifndef COLORS
/** @def RESET
 *  @brief Resets terminal text formatting to default.
 */
#define RESET "\033[0m"          /* Reset */
#define BLACK "\033[30m"         /* Black */
#define RED "\033[31m"           /* Red */
#define GREEN "\033[32m"         /* Green */
#define YELLOW "\033[33m"        /* Yellow */
#define BLUE "\033[34m"          /* Blue */
#define MAGENTA "\033[35m"       /* Magenta */
#define CYAN "\033[36m"          /* Cyan */
#define WHITE "\033[37m"         /* White */
#define BOLDBLACK "\033[1;30m"   /* Bold Black */
#define BOLDRED "\033[1;31m"     /* Bold Red */
#define BOLDGREEN "\033[1;32m"   /* Bold Green */
#define BOLDYELLOW "\033[1;33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1;34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1;35m" /* Bold Magenta */
#define BOLDCYAN "\033[1;36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1;37m"   /* Bold White */

#define BGBLACK "\033[40m"   /* Black */
#define BGRED "\033[41m"     /* Red */
#define BGGREEN "\033[42m"   /* Green */
#define BGYELLOW "\033[43m"  /* Yellow */
#define BGBLUE "\033[44m"    /* Blue */
#define BGMAGENTA "\033[45m" /* Magenta */
#define BGCYAN "\033[46m"    /* Cyan */
#define BGWHITE "\033[47m"   /* White */

#define SUCCESS BOLDGREEN
#define ERROR BOLDRED
#define INFO BOLDBLUE
#define WARNING BOLDYELLOW
#define DEBUG BOLDCYAN

#endif


#ifdef DEBUG
#define DEBUG_PRINT(X) cout << DEBUG << X << RESET;
#define DEBUG_FUNC(X) X;
#define LIMIT 4
#else
#define DEBUG_PRINT(X)
#define DEBUG_FUNC(X);
#define LIMIT 3
#endif


#define BUFFER_SIZE 1024
#define PORT 26000
#define TIMEOUT 10
#define CONTROL_CHAR '*' // ASCII Bell character, used as a control character to indicate end of message

#endif