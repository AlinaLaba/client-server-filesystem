#include "client.h"

bool Client::CreateConnection(const std::string ip, const int port)
{
#ifdef _WIN32
    if(!InitializeWinSock())
        return false;
#endif

    CreateAddrData(ip, port);
    if(!CreateSockAndConnectToServer())
        return false;

    return true;
}

#ifdef _WIN32
bool Client::InitializeWinSock()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return false;

    return true;
}
#endif

void Client::CreateAddrData(const std::string ip, const int port)
{
    addrdata->sin_family = AF_INET;
    addrdata->sin_port = htons(port);
    addrdata->sin_addr.s_addr = inet_addr(ip.c_str());
}

bool Client::CreateSockAndConnectToServer()
{
    *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == *sock)
        return false;

    if (SOCKET_ERROR == connect(*sock, (struct sockaddr*)addrdata, sizeof(*addrdata)))
        return false;

    return true;
}

bool Client::SendMsg(QString &mess) const
{
    char buf[DEFAULT_BUFLEN] = {0};

    strcpy(buf, mess.toStdString().c_str());

    int s = send(*sock, buf, DEFAULT_BUFLEN, 0);
    if (SOCKET_ERROR == s)
        return false;

    return true;
}

bool Client::ReceiveMsg(QString &mess) const
{
    char buf[DEFAULT_BUFLEN + 1] = {0};

    int n = recv(*sock, buf, DEFAULT_BUFLEN, 0);
    if (n == SOCKET_ERROR)
        return false;

    mess = buf;
    return true;
}

bool Client::SendFile(QString path) const
{
    std::ifstream file;
    long long bytessent = 0;
    int dif = 0;

    file.open(path.toStdString(), std::ios::binary | std::ios::ate);
    long long filesize = file.tellg();
    file.seekg(0);

    char buf[DEFAULT_BUFLEN + 1] = {0};

    sprintf(buf, "%lld", filesize);
    dif = send(*sock, buf, DEFAULT_BUFLEN, 0);

    if(dif < 1)
    {
        file.close();
        return false;
    }

    while(bytessent < filesize)
    {
        file.read(buf, DEFAULT_BUFLEN);

        if(file.gcount() != DEFAULT_BUFLEN)
            buf[file.gcount()] = '\0';

        dif = send(*sock, buf, DEFAULT_BUFLEN, 0);
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

bool Client::ReceiveFile(QString &path) const
{
    char buf[DEFAULT_BUFLEN + 1] = {0};
    std::ofstream file;
    int dif = 0;
    long long filesize = 0, morebytes = 0;

    file.open(path.toStdString(), std::ios::binary);

    if(recv(*sock, buf, DEFAULT_BUFLEN, 0) < 1)
    {
        file.close();
        return false;
    }

    filesize = atoll(buf);
    morebytes = filesize;

    while (morebytes > 0)
    {
        dif = recv(*sock, buf, DEFAULT_BUFLEN, 0);

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

bool Client::DetectServer(bool local)
{
#ifdef _WIN32
    InitializeWinSock();
#endif

    sockaddr_in addrdatatmp;
    cSOCKET socktmp = INVALID_SOCKET;

    addrdatatmp.sin_family = AF_INET;

    if(local)
    {
        return CheckPorts(LOCALHOST, addrdatatmp, socktmp);
    }
    else
    {
        std::string ip = "192.168.0.";
        std::string buf;

        for(int i = 1; i < MAX_HOSTS; ++i)
        {
            buf = ip + std::to_string(i);

            if(CheckPorts(buf, addrdatatmp, socktmp))
                return true;
        }

        return false;
    }
}

bool Client::CheckPorts(std::string ip, sockaddr_in addrdatatemp, cSOCKET socktemp)
{
    char buf[DEFAULT_BUFLEN] = {0};
    int bytes = 0;
    unsigned long mode = 1;
    addrdatatemp.sin_addr.s_addr = inet_addr(ip.c_str());

    for(int port = 1; port < MAX_PORTS; ++port)
    {
        socktemp = socket(AF_INET, SOCK_STREAM, 0);
        addrdatatemp.sin_port = htons(port);

        int connres = connect(socktemp, (struct sockaddr*)&addrdatatemp, sizeof(addrdatatemp));
        int err = WSAGetLastError();
        if (SOCKET_ERROR != connres || err == WSAEINPROGRESS)
        {
            QThread::usleep(500);
            mode = 1;
#ifdef _WIN32
            ioctlsocket(socktemp, FIONBIO, &mode);
#else
            ioctl(socktemp, FIONBIO, &mode);
#endif

            bytes = recv(socktemp, buf, DEFAULT_BUFLEN, 0);
            if(bytes > 0)
            {
                CreateAddrData(ip, port);
                CreateSockAndConnectToServer();
                return true;
            }

            shutdown(socktemp, SD_BOTH);

            mode = 0;
#ifdef _WIN32
            ioctlsocket(socktemp, FIONBIO, &mode);
#else
            ioctl(socktemp, FIONBIO, &mode);
#endif
        }
    }

    return false;
}
