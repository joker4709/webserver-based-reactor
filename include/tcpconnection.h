#ifndef TCPCONNECTION_H__
#define TCPCONNECTION_H__

#include "inet_address.h"
#include "socket.h"
#include "socketio.h"
#include <memory>
#include <functional>

class EventLoop;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;

class TcpConnection
    :public std::enable_shared_from_this<TcpConnection>
{
public:
    explicit TcpConnection(int fd, EventLoop *loop);

    ~TcpConnection();

    void send(const string &msg);

    void sendInLoop(const string &msg);
    
    string receive();

    string toString();

    void setNewConnectionCallback(const TcpConnectionCallback &);

    void setMessageCallback(const TcpConnectionCallback &);

    void setCloseCallback(const TcpConnectionCallback &);

    void handleNewConnectionCallback();

    void handleMessageCallback();

    void handleCloseCallback();

    bool isClosed();
private:
    InetAddress getLocalAddr();

    InetAddress getPeerAddr();
private:
    EventLoop *loop_;
    Socket sock_;
    SocketIO sockIO_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    TcpConnectionCallback OnNewConnectionCb_;
    TcpConnectionCallback OnCloseCb_;
    TcpConnectionCallback OnMessageCb_;
};



#endif