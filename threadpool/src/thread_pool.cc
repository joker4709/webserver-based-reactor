#include "thread_pool.h"
#include "task_queue.h"
#include <cstddef>
#include <functional>
#include <iostream>
#include <unistd.h>
 

ThreadPool::ThreadPool(size_t num, size_t size)
    :threadNum_(num),
    queSize(size),
    threads_(),
    que_(size),
    isExit_(false)
{

}

void ThreadPool::start()
{
    for(int i = 0; i < threadNum_; ++i) {
        threads_.emplace_back(thread(std::bind(&ThreadPool::doTask, this)));
    }
}

void ThreadPool::stop()
{
    isExit_ = true;
    for(auto &th: threads_) {
        th.join();
    }
}

void ThreadPool::addTask(Task &&task)
{
    if(task) {
        que_.push(task);
    }
}

ElemType ThreadPool::getTask()
{
    return que_.pop();
}

void ThreadPool::doTask()
{
    while(!isExit_){
        auto task = getTask();
        if(task) {
            task();
        }
        else {
            std::cout << "nullptr == ptask" << std::endl;
        }
    }
}