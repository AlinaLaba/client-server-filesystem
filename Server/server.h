#ifndef SERVER_H
#define SERVER_H

#include "servertypes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#define DEFAULT_PORT 24042
#define DEFAULT_BUFLEN 1024

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

class Server
{
private:
    cSOCKET* sock;
    sockaddr_in* sockAddr;

public:
    #ifdef _WIN32
    bool InitializeWinSock();
    #endif

    Server() : sock(new cSOCKET), sockAddr(new sockaddr_in) {}
    bool CreateConnection();
    void CreateAddrData();
    bool CreateSocket();
    bool BindSocket();
    bool ListenSocket();
    bool AcceptConnection(cSOCKET* ClientSock);
    bool ReceiveMsg(cSOCKET* ClientSock, std::string &mess) const;
    bool SendMsg(cSOCKET* ClientSock, std::string &mess) const;
    bool SendFile(cSOCKET* ClientSock, std::string &path) const;
    bool ReceiveFile(cSOCKET* ClientSock, std::string &path) const;
};

#endif // SERVER_H
