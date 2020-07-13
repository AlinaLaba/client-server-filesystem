#ifndef CONNVIEWCONTROLLER_H
#define CONNVIEWCONTROLLER_H

#include "client.h"
#include <iostream>
#include <QMainWindow>

class ConnViewController
{
private:
    Client* client;

public:
    ConnViewController(Client* _client) : client(_client) {}
    void startConn(std::string ip, int port) const;
    bool autoConnect(bool local) const;
};

#endif // CONNVIEWCONTROLLER_H
