#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__

#include "acceptor.h"
#include "event_loop.h"
#include "tcpconnection.h"

class TcpServer
{
public:
    TcpServer(const std::string &ip, unsigned short port);

    ~TcpServer();

    void setAllCallback(TcpConnectionCallback &&cb1, TcpConnectionCallback &&cb2, TcpConnectionCallback &&cb3);

    void start();
    
    void stop();
private:
    Acceptor acpt_;
    EventLoop loop_;
};
#endif