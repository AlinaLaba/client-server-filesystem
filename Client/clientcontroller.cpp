#include "clientcontroller.h"

void ClientController::getListFromServer(QString &buf) const
{
    client->ReceiveMsg(buf);

    if(buf.left(5) == "Hello")
        client->ReceiveMsg(buf);

    QStringList str;
    model->setPath(buf);

    client->ReceiveMsg(buf);
    str.clear();
    str = buf.split("\n");
    model->setName(str);

    client->ReceiveMsg(buf);
    str.clear();
    str = buf.split("\n");
    model->setType(str);

    client->ReceiveMsg(buf);
    str.clear();
    str = buf.split("\n");
    model->setChange_time(str);

    client->ReceiveMsg(buf);
    str.clear();
    str = buf.split("\n");
    model->setFile_size(str);
}

void ClientController::sendCommand(int command, QString name, QString path) const
{
    QString buf;

    if(command == 1)
        buf.append("<Copy> " + name + "|" + path);
    else if(command == 2)
        buf.append("<Move> " + name + "|" + path);
    else if(command == 3)
        buf.append("<Rena> " + name + "|" + path);
    else if(command == 4)
        buf.append("<Dele> " + name);
    else if(command == 5)
        buf.append("<CdUp>");
    else if(command == 6)
        buf.append("<Cdir> " + name);
    else if(command == 7)
        buf.append("<Send> " + name);
    else if(command == 8)
        buf.append("<Recv> " + name);
    else if(command == 9)
        buf.append("<Updt>");

    client->SendMsg(buf);
}

bool ClientController::getRespond() const
{
    QString mesg;
    client->ReceiveMsg(mesg);
    if(mesg == "true")
        return true;
    else
        return false;
}

bool ClientController::SendFile(QString path) const
{
    return client->SendFile(path);
}

bool ClientController::ReceiveFile(QString path) const
{
    return client->ReceiveFile(path);
}
