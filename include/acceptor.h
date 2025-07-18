#ifndef ACCEPTOR_H__
#define ACCEPTOR_H__

#include "socket.h"
#include "inet_address.h"
#include <cstddef>


class Acceptor
{
public:
    Acceptor(const string &ip, unsigned short port);
    ~Acceptor();

    void ready();
    
    int accept();

    int fd();
private:
    void bind();
    void listen(size_t listenSize = 1024);
    void setAddrReuse();
    void setPortReuse();
private:
    Socket sock_;
    InetAddress addr_;
};


#endif