#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <fstream>
#include <thread>
#include <QThread>
#include "clienttypes.h"
#define DEFAULT_BUFLEN 1024
#define MAX_PORTS 65535
#define MAX_HOSTS 255
#define LOCALHOST "127.0.0.1"

class Client
{
private:
    cSOCKET *sock;
    sockaddr_in *addrdata;

public:
#ifdef _WIN32
    bool InitializeWinSock();
#endif

    Client() : sock(new cSOCKET), addrdata(new sockaddr_in) {}
    void CreateAddrData(const std::string ip, const int port);
    bool CreateSockAndConnectToServer();
    bool SendMsg(QString &mess) const;
    bool ReceiveMsg(QString &mess) const;
    bool CreateConnection(const std::string ip, const int port);
    bool SendFile(QString path) const;
    bool ReceiveFile(QString &path) const;
    bool DetectServer(bool local);
    bool CheckPorts(std::string ip, sockaddr_in addrdatatemp, cSOCKET socktemp);
};

#endif // CLIENT_H
