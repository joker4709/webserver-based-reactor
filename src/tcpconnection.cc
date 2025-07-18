#include "tcpconnection.h"
#include "event_loop.h"
#include "inet_address.h"
#include <arpa/inet.h>
#include <functional>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <iostream>

TcpConnection::TcpConnection(int fd, EventLoop *loop):
    loop_(loop),
    sock_(fd), 
    sockIO_(fd),
    localAddr_(getLocalAddr()),
    peerAddr_(getPeerAddr())
{

}

void TcpConnection::send(const string &msg)
{
    sockIO_.writen(msg.c_str(), msg.size());
}

string TcpConnection::receive()
{
    char buf[65535];
    sockIO_.readn(buf, sizeof buf);
    return buf;
}

string TcpConnection::toString()
{
    std::ostringstream oss;
    oss << peerAddr_.getIP() << ":" << peerAddr_.getPort() <<
    " ----> " << localAddr_.getIP() << ":" << localAddr_.getPort();
    return oss.str();
}

InetAddress TcpConnection::getLocalAddr() 
{
    struct sockaddr_in localAddr;
    socklen_t len = sizeof(localAddr);
    int ret = getsockname(sock_.fd(), (struct sockaddr *)&localAddr, &len);
    if(ret == -1) {
        std::cerr << "getsockname" << '\n';
        exit(1);
    }
    return InetAddress(localAddr);
}

InetAddress TcpConnection::getPeerAddr()
{
    struct sockaddr_in peerAddr;
    socklen_t len = sizeof(peerAddr);
    int ret = getpeername(sock_.fd(), (struct sockaddr *)&peerAddr, &len);
    if(ret == -1) {
        std::cerr << "getpeername" << '\n';
        exit(1);
    }
    return InetAddress(peerAddr);
}

TcpConnection::~TcpConnection() 
{

}

void TcpConnection::setNewConnectionCallback(const TcpConnectionCallback & cb) 
{
    OnNewConnectionCb_ = cb;
}

void TcpConnection::setMessageCallback(const TcpConnectionCallback & cb) 
{
    OnMessageCb_ = cb;
}

void TcpConnection::setCloseCallback(const TcpConnectionCallback & cb) 
{
    OnCloseCb_ = cb;
}

void TcpConnection::handleNewConnectionCallback()
{
    if(OnNewConnectionCb_) {
        OnNewConnectionCb_(shared_from_this());
    }
    else {
        throw "OnNewConnectionCb_ is nullptr.";
    }
}

void TcpConnection::handleMessageCallback()
{
    if(OnMessageCb_) {
        OnMessageCb_(shared_from_this());
    }
    else {
        throw "OnMessageCb_ is nullptr.";
    }
}

void TcpConnection::handleCloseCallback()
{
    if(OnCloseCb_) {
        OnCloseCb_(shared_from_this());
    }
    else {
        throw "OnCloseCb_ is nullptr.";
    }
}

bool TcpConnection::isClosed()
{
    char buf[10] = {0};
    int ret = ::recv(sock_.fd(), buf, 10, MSG_PEEK | MSG_DONTWAIT);
    return (0 == ret);
}

void TcpConnection::sendInLoop(const string &msg)
{
    if(loop_) {
        loop_->runInLoop(std::bind(&TcpConnection::send, this, msg));
    }
}