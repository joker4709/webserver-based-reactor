#include "socket.h"
#include <iostream>

Socket::Socket() 
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(fd_ == -1) {
        std::cerr << "socket" << '\n';
        exit(1);
    }
}

Socket::Socket(int fd): fd_(fd) 
{

}

int Socket::fd() 
{
    return fd_;
}

Socket::~Socket()
{

}