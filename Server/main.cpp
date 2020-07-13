#include <QCoreApplication>
#include <thread>
#include "server.h"
#include "filemanager.h"

void StartConnection(cSOCKET *clsock, FileManager *fm, Server* server)
{
    std::string buf = "Hello";
    server->SendMsg(clsock, buf);
    fm->sendList(server, clsock);

    while(true)
    {
        if(!fm->getCommand(server, clsock))
        {
            shutdown(*clsock, SD_BOTH);

            delete fm;
            delete clsock;

            return;
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
    std::vector<cSOCKET*> clSocket;
    std::vector<FileManager*> fm;

    if(server.CreateConnection())
        std::cout<<"Prepared to connect" << std::endl;

    clSocket.push_back(new cSOCKET);
    while(server.AcceptConnection(clSocket.back()))
    {
        fm.push_back(new FileManager);

        std::thread t(StartConnection, clSocket.back(), fm.back(), &server);
        t.detach();

        clSocket.push_back(new cSOCKET);
    }

    return a.exec();
}
