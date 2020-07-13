#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include "clientmodel.h"

class ClientController
{
private:
    ClientModel *model;
    Client *client;

public:
    ClientController(ClientModel* _model, Client* _client) : model(_model), client(_client) {}
    void getListFromServer(QString &buf) const;
    void sendCommand(int command, QString name = "", QString path = "") const;
    bool getRespond() const;
    bool SendFile(QString path) const;
    bool ReceiveFile(QString path) const;
};

#endif // CLIENTCONTROLLER_H
