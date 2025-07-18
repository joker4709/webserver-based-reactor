#include "EchoServer.h"
#include "tcp_server.h"
#include "tcpconnection.h"
#include <functional>
#include <iostream>

MyTask::MyTask(const std::string msg, const TcpConnectionPtr &conn)
    :msg_(msg),
    conn_(conn)
{

}

void MyTask::process()
{
    conn_->sendInLoop(msg_);
}

EchoServer::EchoServer(size_t threadNum, size_t queSize, const std::string &ip, unsigned short port)

    :pool_(threadNum, queSize),
    server_(ip, port)
{

}

EchoServer::~EchoServer()
{

}

void EchoServer::start()
{
    using namespace std::placeholders;
    server_.setAllCallback(std::bind(&EchoServer::onNewConnection, this, _1), 
        std::bind(&EchoServer::onMessage, this, _1),
        std::bind(&EchoServer::onClose, this, _1));

    pool_.start();
    server_.start();
}

void EchoServer::stop()
{
    pool_.stop();
    server_.stop();
}

void EchoServer::onNewConnection(const TcpConnectionPtr &con)
{
    std::cout << con->toString() << " established!" << '\n';
}

void EchoServer::onMessage(const TcpConnectionPtr &con)
{
    string msg = con->receive();

    std::cout << "Client: " << msg;

    MyTask task(msg, con);
    pool_.addTask(std::bind(&MyTask::process, task));
}

void EchoServer::onClose(const TcpConnectionPtr &con)
{
    std::cout << con->toString() << " disconnected!" << '\n';
}