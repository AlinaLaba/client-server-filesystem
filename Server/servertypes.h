#ifndef SERVERTYPES_H
#define SERVERTYPES_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET cSOCKET;

#elif __linux__
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef int cSOCKET;

#define SD_BOTH 2
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

typedef void(*pSendMsg)(cSOCKET*, char*);

typedef bool(*pReceiveMsg)(cSOCKET*, char*);

#endif // SERVERTYPES_H
