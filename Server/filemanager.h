#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QDir>
#include <fstream>
#include <QFileDevice>
#include <QDateTime>
#include "server.h"

class FileManager
{
private:
    QDir currentdir;

public:
    FileManager();
    QFileInfoList getFileList() const;
    QString findAbsolutePath(QString filename) const;
    bool cdUp();
    bool cd(QString path);
    bool copy(QString sourcepath, QString destpath);
    bool move(QString sourcepath, QString destpath);
    bool rename(QString oldname, QString newname);
    bool remove(QString filename);
    void sendList(Server* server, cSOCKET* clsock) const;
    bool getCommand(Server* server, cSOCKET* clsock);
    void sendRespond(bool value, Server* server, cSOCKET* clsock) const;
    bool updateList();
};

#endif // FILEMANAGER_H
