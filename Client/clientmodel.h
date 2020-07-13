#ifndef CLIENTMODEL_H
#define CLIENTMODEL_H

#include <QStringList>
#include "client.h"

class ClientModel
{
private:
    QString path;
    QStringList name;
    QStringList type;
    QStringList change_time;
    QStringList file_size;
    Client* client;

public:
    ClientModel(Client* _client) : client(_client) {}
    void setPath(QString &_path);
    void setName(QStringList &_name);
    void setType(QStringList &_type);
    void setChange_time(QStringList &_change_time);
    void setFile_size(QStringList &_file_size);
    QString getPath() const;
    QStringList getName() const;
    QStringList getType() const;
    QStringList getChange_time() const;
    QStringList getFile_size() const;
};

#endif // CLIENTMODEL_H
