#include "server.h"

bool Server::CreateConnection()
{
#ifdef _WIN32
    if(!InitializeWinSock())
    {
        std::cout << "WinSock initialization failed";
        return false;
    }
#endif

    CreateAddrData();

    if(!CreateSocket())
    {
        std::cout << "Socket creation failed";
        return false;
    }

    if(!BindSocket())
    {
        std::cout << "Socket binding failed";
        return false;
    }

    if(!ListenSocket())
    {
        std::cout << "Socket listening failed";
        return false;
    }

    return true;
}

#ifdef _WIN32
bool Server::InitializeWinSock()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return false;

    return true;
}
#endif

void Server::CreateAddrData()
{
    sockAddr->sin_family = AF_INET;
    sockAddr->sin_port = htons(DEFAULT_PORT);
    sockAddr->sin_addr.s_addr = INADDR_ANY;
}

bool Server::CreateSocket()
{
    *sock = socket(sockAddr->sin_family, SOCK_STREAM, IPPROTO_TCP);

    if (INVALID_SOCKET == *sock)
        return false;

    return true;
}

bool Server::BindSocket()
{
    if (SOCKET_ERROR == bind(*sock, (sockaddr*)(sockAddr), sizeof(*sockAddr)))
        return false;

    return true;
}

bool Server::ListenSocket()
{
    if (SOCKET_ERROR == listen(*sock, SOMAXCONN))
        return false;

    return true;
}

bool Server::AcceptConnection(cSOCKET* ClientSock)
{
    struct sockaddr_in cliAddr;
    socklen_t addrlen = sizeof(cliAddr);
    *ClientSock = accept(*sock, (sockaddr*)&cliAddr, &addrlen);

    if (INVALID_SOCKET == *ClientSock)
        return false;

    return true;
}

bool Server::ReceiveMsg(cSOCKET* ClientSock, std::string &mess) const
{
    char buf[DEFAULT_BUFLEN + 1] = {0};

    if (recv(*ClientSock, buf, DEFAULT_BUFLEN, 0) > 0)
    {
        mess = buf;
        return true;
    }

    mess = buf;
    return false;
}

bool Server::SendMsg(cSOCKET* ClientSock, std::string &mess) const
{
    char buf[DEFAULT_BUFLEN] = {0};
    mess.copy(buf, mess.size() + 1);

    if (send(*ClientSock, buf, DEFAULT_BUFLEN, 0) < 1)
        return false;

    return true;
}

bool Server::SendFile(cSOCKET* ClientSock, std::string &path) const
{
    std::ifstream file;
    long long bytessent = 0;
    int dif = 0;

    file.open(path, std::ios::binary | std::ios::ate);
    long long filesize = file.tellg();
    file.seekg(0);

    char buf[DEFAULT_BUFLEN + 1] = {0};

    sprintf(buf, "%lld", filesize);

    if(send(*ClientSock, buf, DEFAULT_BUFLEN, 0) < 1)
    {
        file.close();
        return false;
    }

    while(bytessent < filesize)
    {
        file.read(buf, DEFAULT_BUFLEN);

        dif = send(*ClientSock, buf, DEFAULT_BUFLEN, 0);
        bytessent += dif;

        if(dif < 1)
        {
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

bool Server::ReceiveFile(cSOCKET* ClientSock, std::string &path) const
{
    char buf[DEFAULT_BUFLEN + 1] = {0};
    std::ofstream file;
    int dif = 0;
    long long filesize = 0, morebytes = 0;

    file.open(path, std::ios::binary);

    if(recv(*ClientSock, buf, DEFAULT_BUFLEN, 0) < 1)
    {
        file.close();
        return false;
    }

    filesize = atoll(buf);
    morebytes = filesize;

    while (morebytes > 0)
    {
        dif = recv(*ClientSock, buf, DEFAULT_BUFLEN, 0);

        if(dif < 1 && morebytes > 0)
        {
            file.close();
            return false;
        }

        morebytes -= dif;

        if(morebytes < 0)
            file.write(buf, morebytes + DEFAULT_BUFLEN);
        else
            file.write(buf, dif);

        memcpy(buf, "\0", DEFAULT_BUFLEN);
    }
    return true;
}
