#include "clientmodel.h"

void ClientModel::setPath(QString &_path)
{
    path = _path;
}

void ClientModel::setName(QStringList &_name)
{
    name = _name;
}

void ClientModel::setType(QStringList &_type)
{
    type = _type;
}

void ClientModel::setChange_time(QStringList &_change_time)
{
    change_time = _change_time;
}

void ClientModel::setFile_size(QStringList &_file_size)
{
    file_size = _file_size;
}

QString ClientModel::getPath() const
{
    return path;
}

QStringList ClientModel::getName() const
{
    return name;
}

QStringList ClientModel::getType() const
{
    return type;
}

QStringList ClientModel::getChange_time() const
{
    return change_time;
}

QStringList ClientModel::getFile_size() const
{
    return file_size;
}
