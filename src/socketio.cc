#include "socketio.h"
#include <strings.h>
#include <sys/socket.h>

SocketIO::SocketIO(int fd):fd_(fd)
{

}

int SocketIO::readn(char *buf, int len)
{
    bzero(buf, len);
    int ret = recv(fd_, buf, len, 0);
    return ret;
}

int SocketIO::readLine(char *buf, int len)
{
    int ret = recv(fd_, buf, len, 0);
    return ret;
}


int SocketIO::writen(const char *buf, int len)
{
    int ret = send(fd_, buf, len, 0);
    return ret;
}

SocketIO::~SocketIO()
{
    
}