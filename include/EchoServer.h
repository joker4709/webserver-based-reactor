#pragma once

#include "tcp_server.h"
#include "tcpconnection.h"
#include "thread_pool.h"
#include <cstddef>

class MyTask
{
public:
    MyTask(const std::string msg, const TcpConnectionPtr &conn);
    void process();

private:
    std::string msg_;
    TcpConnectionPtr conn_;
};

class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize, const std::string &ip, unsigned short port);

    ~EchoServer();

    void start();

    void stop();

    void onNewConnection(const TcpConnectionPtr &con);

    void onMessage(const TcpConnectionPtr &con);

    void onClose(const TcpConnectionPtr &con);

private:
    ThreadPool pool_;
    TcpServer server_;
};