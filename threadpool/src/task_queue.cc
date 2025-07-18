#include "task_queue.h"
#include <cstddef>
#include <memory>
#include <mutex>

TaskQueue::TaskQueue(size_t queSize): queSize_(queSize),
    que_(),
    mtx_(),
    notFull_(),
    notEmpty_()
{

}

TaskQueue::~TaskQueue()
{

}

void TaskQueue::push(ElemType ptask)
{
    std::unique_lock<mutex> ul(mtx_);
    while (full()) {
        notFull_.wait(ul);
    }
    que_.push(ptask);
    notEmpty_.notify_one();
}

ElemType TaskQueue::pop() 
{
    std::unique_lock<mutex> ul(mtx_);
    while(empty()) {
        notEmpty_.wait(ul);
    }
    auto elem = que_.front();
    que_.pop();
    notFull_.notify_one();

    return elem;
}


bool TaskQueue::empty() const
{
    return que_.size() == 0;
}

bool TaskQueue::full() const
{
    return queSize_ == que_.size();
}