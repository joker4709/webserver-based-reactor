#ifndef EVENT_LOOP_H__
#define EVENT_LOOP_H__

#include "acceptor.h"
#include "tcpconnection.h"
#include "timer.h"
#include <bits/types/time_t.h>
#include <vector>
#include <sys/socket.h>
#include <map>
#include <mutex>
#include <functional>

using std::mutex;
using std::function;


using Functor = function<void()>;

class EventLoop
{
public:
    EventLoop(Acceptor &acceptor);

    ~EventLoop();

    void loop();

    void unloop();

    void setNewConnectionCallback(TcpConnectionCallback &&);

    void setCloseCallback(TcpConnectionCallback &&);

    void setMessageCallback(TcpConnectionCallback &&);

    int createEventFd();

    void handleRead();

    void wakeup();

    void doPengdingFunctors();

    void runInLoop(Functor &&);
private:
    int createEpollFd();

    void waitEpollFd(time_t timeoutMS);

    void handleNewConnection();

    void handleMessage(int fd);

    void addEpollReadFd(int fd);

    void delEpollReadFd(int fd);

    void closeConnection(int fd);

private:
    int epfd_;
    bool isLooping_;
    Acceptor &acceptor_;
    std::vector<struct epoll_event> evtList_;
    mutex mtx_;
    int evtfd_;
    std::vector<Functor> pengdings_;
    std::map<int, std::shared_ptr<TcpConnection>> conns_;
    time_t expire_ = 10000;
    Utils::Timer::Timer timer_;
    TcpConnectionCallback OnNewConnectionCb_;
    TcpConnectionCallback OnCloseCb_;
    TcpConnectionCallback OnMessageCb_;
};

#endif