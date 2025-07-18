#include "tcp_server.h"
#include "event_loop.h"
#include "tcpconnection.h"


TcpServer::TcpServer(const std::string &ip, unsigned short port): acpt_(ip, port), loop_(acpt_)
{

}

TcpServer::~TcpServer()
{

}

void TcpServer::setAllCallback(TcpConnectionCallback &&cb1, TcpConnectionCallback &&cb2, TcpConnectionCallback &&cb3)
{
    loop_.setNewConnectionCallback(std::move(cb1));
    loop_.setMessageCallback(std::move(cb2));
    loop_.setCloseCallback(std::move(cb3));
}

void TcpServer::start()
{
    acpt_.ready();
    loop_.loop();
}

void TcpServer::stop()
{
    loop_.unloop();
}