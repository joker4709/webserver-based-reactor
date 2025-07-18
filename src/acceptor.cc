#include "acceptor.h"
#include "inet_address.h"
#include "socket.h"
#include <asm-generic/socket.h>
#include <cstddef>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

Acceptor::Acceptor(const string &ip, unsigned short port):sock_(), addr_(ip, port)
{

}

void Acceptor::setAddrReuse() 
{
    int opt = 1;
    int ret = setsockopt(sock_.fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(ret == -1) {
        std::cerr << "setsockopt" << '\n';
    }
}

void Acceptor::setPortReuse()
{
    int opt = 1;
    int ret = setsockopt(sock_.fd(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if(ret == -1) {
        std::cerr << "setsockopt" << '\n';
    }
}

void Acceptor::bind() 
{
    int ret = ::bind(sock_.fd(), (struct sockaddr *)addr_.getInetAddressPtr(), sizeof(sockaddr));
    if(ret == -1) {
        std::cerr << "bind" << '\n';
        exit(1);
    }
}

void Acceptor::listen(size_t listenSize)
{
    int ret = ::listen(sock_.fd(), listenSize);
    if(ret == -1) {
        std::cerr << "listen" << '\n';
        exit(1);
    }
}

int Acceptor::accept()
{
    int fd = ::accept(sock_.fd(), nullptr, nullptr);
    if(fd == -1) {
        std::cerr << "accept" << '\n';
        return -1;
    }
    return fd;
}

void Acceptor::ready()
{
    setAddrReuse();
    setPortReuse();
    bind(); 
    listen();
}

Acceptor::~Acceptor()
{

}

int Acceptor::fd() 
{
    return sock_.fd();
}