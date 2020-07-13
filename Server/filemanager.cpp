#include "filemanager.h"

FileManager::FileManager()
{
    currentdir.home();
}

QFileInfoList FileManager::getFileList() const
{
    return currentdir.entryInfoList();
}

bool FileManager::cdUp()
{
    return currentdir.cdUp();
}

bool FileManager::cd(QString path)
{
    if(currentdir.cd(path))
        return true;
    else
        return false;
}

QString FileManager::findAbsolutePath(QString filename) const
{
    QFileInfoList list = getFileList();

    for(int i = 0; i < list.size(); ++i)
    {
        if(list.at(i).fileName() == filename)
            return list.at(i).absoluteFilePath();
    }

    return "";
}

bool FileManager::copy(QString sourcepath, QString destpath)
{
    return QFile::copy(findAbsolutePath(sourcepath), destpath);
}

bool FileManager::move(QString sourcepath, QString destpath)
{
    if(QFile::copy(findAbsolutePath(sourcepath), destpath))
        return QFile::remove(findAbsolutePath(sourcepath));

    return false;
}

bool FileManager::rename(QString oldname, QString newname)
{
    return QFile::rename(findAbsolutePath(oldname), newname);
}

bool FileManager::remove(QString filename)
{
    return QFile::remove(findAbsolutePath(filename));
}

void FileManager::sendList(Server* server, cSOCKET* clsock) const
{
    QFileInfoList list = getFileList();

    std::string name;
    std::string type;
    std::string change_time;
    std::string file_size;

    for(int i = 0; i < list.size(); ++i)
    {
        if(list.at(i).fileName().toStdString() != "." && list.at(i).fileName().toStdString() != "..")
        {
            name += list.at(i).fileName().toStdString() + "\n";

            if(list.at(i).isFile())
                type += "File\n";
            else
                type += "Dir\n";

            change_time += list.at(i).fileTime(QFileDevice::FileModificationTime).toString().toStdString() + "\n";

            if(list.at(i).size() != 0)
                file_size += std::to_string(list.at(i).size() / 1024 + 1) + "\n";
            else
                file_size += "0\n";
        }
    }

    std::string path = currentdir.absolutePath().toStdString();
    server->SendMsg(clsock, path);
    server->SendMsg(clsock, name);
    server->SendMsg(clsock, type);
    server->SendMsg(clsock, change_time);
    server->SendMsg(clsock, file_size);
}

void FileManager::sendRespond(bool value, Server* server, cSOCKET* clsock) const
{
    std::string buf;
    if(value)
    {
        buf.assign("true");
        server->SendMsg(clsock, buf);
    }
    else
    {
        buf.assign("false");
        server->SendMsg(clsock, buf);
    }
}

bool FileManager::getCommand(Server* server, cSOCKET* clsock)
{
    std::string buf;
    server->ReceiveMsg(clsock, buf);

    if(std::string::npos != buf.find("<Copy> "))
    {
        buf.erase(0, 7);

        QString name = QString::fromStdString(buf);
        sendRespond(copy(name.split("|").at(0), name.split("|").at(1)), server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<Move> "))
    {
        buf.erase(0, 7);

        QString name = QString::fromStdString(buf);
        sendRespond(move(name.split("|").at(0), name.split("|").at(1)), server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<Rena> "))
    {
        buf.erase(0, 7);

        QString name = QString::fromStdString(buf);
        sendRespond(rename(name.split("|").at(0), name.split("|").at(1)), server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<Dele> "))
    {
        buf.erase(0, 7);

        sendRespond(remove(QString::fromStdString(buf)), server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<CdUp>"))
    {
        if(cdUp())
        {
            sendRespond(true, server, clsock);
            sendList(server, clsock);
        }
        else
            sendRespond(false, server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<Cdir> "))
    {
        buf.erase(0, 7);

        if(cd(QString::fromStdString(buf)))
        {
            sendRespond(true, server, clsock);
            sendList(server, clsock);
        }
        else
            sendRespond(false, server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<Send> "))
    {
        buf.erase(0, 7);

        sendRespond(server->SendFile(clsock, buf), server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<Recv> "))
    {
        buf.erase(0, 7);

        sendRespond(server->ReceiveFile(clsock, buf), server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<Updt>"))
    {
        if(updateList())
        {
            sendRespond(true, server, clsock);
            sendList(server, clsock);
        }
        else
            sendRespond(false, server, clsock);

        return true;
    }
    else if(std::string::npos != buf.find("<Close>"))
        return false;

    return false;
}

bool FileManager::updateList()
{
    if(cd(currentdir.path()))
        return true;
    else
        return false;
}
