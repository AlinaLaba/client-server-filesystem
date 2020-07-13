#ifndef CLIENTTYPES_H
#define CLIENTTYPES_H

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
#include <sys/ioctl.h>

typedef int cSOCKET;

#define SD_BOTH 2
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

typedef void(*pReceiveMsg)(cSOCKET*, char*);

typedef void(pSendList)(cSOCKET*, char*);

#endif // CLIENTTYPES_H
