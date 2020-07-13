#include "connviewcontroller.h"

void ConnViewController::startConn(std::string ip, int port) const
{
    client->CreateConnection(ip, port);
}

bool ConnViewController::autoConnect(bool local) const
{
    return client->DetectServer(local);
}
