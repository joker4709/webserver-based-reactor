#pragma once

#include "task_queue.h"
#include "tcpconnection.h"
#include <cstddef>
#include <functional>
#include <vector>
#include <thread>

using std::thread;
using std::function;

using Task = function<void()>;

class ThreadPool
{
public:
    ThreadPool(size_t num, size_t size);

    void start();

    void stop();

    void addTask(Task &&task);
private:
    Task getTask();

    void doTask();
private:
    size_t threadNum_;
    size_t queSize;
    std::vector<thread> threads_;
    TaskQueue que_;
    bool isExit_;
};