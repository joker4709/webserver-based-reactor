#ifndef SOCKET_H__
#define SOCKET_H__

#include <sys/socket.h>

class Socket
{
public:
    Socket();
    explicit Socket(int);
    int fd();

    ~Socket();
private:
    int fd_;
};



#endif