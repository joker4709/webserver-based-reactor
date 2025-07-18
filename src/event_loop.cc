#include "event_loop.h"
#include "acceptor.h"
#include "tcpconnection.h"
#include "timer.h"
#include <bits/stdint-uintn.h>
#include <cerrno>
#include <cstdio>
#include <memory>
#include <mutex>
#include <sys/epoll.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/eventfd.h>

EventLoop::EventLoop(Acceptor &acceptor):
    epfd_(createEpollFd()), 
    isLooping_(false),
    acceptor_(acceptor), 
    evtList_(1024),
    evtfd_(createEventFd()),
    mtx_(),
    timer_()
{
    int listenfd = acceptor.fd();
    addEpollReadFd(listenfd);

    addEpollReadFd(evtfd_);
}

void EventLoop::loop()
{
    isLooping_ = true;
    while(isLooping_) {
        time_t timeoutMS = -1;
        if (expire_ > 0) {
            timeoutMS = timer_.getNextTick();
        }
        waitEpollFd(timeoutMS);
    }
}


void EventLoop::unloop()
{
    isLooping_ = false;
}

int EventLoop::createEpollFd()
{
    int ret = epoll_create(1024);
    if(ret == -1) {
        std::cerr << "epoll_create" << '\n';
        return -1;
    }
    return ret;
}

void EventLoop::waitEpollFd(time_t timeoutMS) 
{
    int nready = 0;
    do {
        nready = epoll_wait(epfd_, &*evtList_.begin(), evtList_.size(), timeoutMS);
    }while(-1 == nready && errno == EINTR);

    if(nready == -1) {
        std::cerr << "epoll_wait" << '\n';
        return;
    }
    else if (nready == 0) {
        return;
    }
    else {
        if(nready == (int)evtList_.size()) {
            evtList_.resize(2 * nready);
        }

        for(int i = 0; i < nready; ++i) {
            int fd = evtList_[i].data.fd;
            int listenfd = acceptor_.fd();

            if(fd == listenfd) { //处理新连接到来
                if(evtList_[i].events & EPOLLIN) {
                    handleNewConnection();
                }
            }
            else if(fd == evtfd_){ //处理进程通信事件到来
                if(evtList_[i].events & EPOLLIN){
                    handleRead();

                    doPengdingFunctors();
                }
            }
            else { //老连接有消息到来
                if(evtList_[i].events & EPOLLIN) {
                    handleMessage(fd);
                }
            }
        }
    }
}

void EventLoop::addEpollReadFd(int fd) 
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
    if(ret == -1) {
        std::cerr << "epoll_ctl" << '\n';
        exit(1);
    }
}

void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &ev);
    if(ret == -1) {
        std::cerr << "epoll_ctl" << '\n';
        exit(1);
    }
}

void EventLoop::setNewConnectionCallback(TcpConnectionCallback &&cb)
{
    OnNewConnectionCb_ = std::move(cb);
}

void EventLoop::setCloseCallback(TcpConnectionCallback &&cb)
{
    OnCloseCb_ = std::move(cb);
}

void EventLoop::setMessageCallback(TcpConnectionCallback &&cb)
{
    OnMessageCb_ = std::move(cb);
}

void EventLoop::handleNewConnection()
{
    int connfd = acceptor_.accept();
    if(connfd == -1) {
        exit(1);
    }
    addEpollReadFd(connfd);
    timer_.addTask(connfd, expire_, std::bind(&EventLoop::closeConnection, this, connfd));
    auto conn = std::make_shared<TcpConnection>(connfd, this);
    conn->setNewConnectionCallback(OnNewConnectionCb_);
    conn->setMessageCallback(OnMessageCb_);
    conn->setCloseCallback(OnCloseCb_);
    conns_[connfd] = conn;

    conn->handleNewConnectionCallback();
}

void EventLoop::closeConnection(int fd)
{
    auto it = conns_.find(fd);
    if (it != conns_.end()) {
        auto conn = it->second;

        if(conn->isClosed()) {
            conn->handleCloseCallback();
            delEpollReadFd(fd);
            conns_.erase(it);
        }
        else {
            std::cout << conns_[fd]->toString() << " timeout, be kicked out." << '\n';
            delEpollReadFd(fd);
            conns_.erase(it);
            close(fd);
        }
    }
    else {
        std::cerr << __func__ << ": connection is not exist." << '\n';
        return;
    }
}

void EventLoop::handleMessage(int fd)
{
    auto it = conns_.find(fd);
    if(it != conns_.end()) {
        auto conn = it->second;

        if(conn->isClosed()) {
            conn->handleCloseCallback();
            delEpollReadFd(fd);
            conns_.erase(it);
        }
        else {
            timer_.adjust(fd, expire_);
            conn->handleMessageCallback();
        }
    }
    else {
        std::cerr << "connection is not exist." << '\n';
        return;
    }
}

EventLoop::~EventLoop()
{
    close(epfd_);
    close(evtfd_);
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    int ret = read(evtfd_, &one, sizeof(one));

    if(ret != sizeof(one)) {
        perror("handleRead");
        return;
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t ret = write(evtfd_, &one, sizeof(uint64_t));
    if(ret != sizeof(uint64_t)) {
        perror("wakeup");
        return;
    }
}

void EventLoop::doPengdingFunctors()
{
    std::vector<Functor> tmp;
    {
        std::lock_guard<mutex> lg(mtx_);
        tmp.swap(pengdings_);
    }

    for(auto &cb : tmp) {
        cb();
    }
}

void EventLoop::runInLoop(Functor &&cb) 
{
    {
        std::lock_guard<mutex> lg(mtx_);
        pengdings_.emplace_back(std::move(cb));
    }

    wakeup();
}

int EventLoop::createEventFd()
{
    int ret = eventfd(10, 0);
    if(ret < 0) {
        perror("eventfd");
        return ret;
    }
    return ret;
}
