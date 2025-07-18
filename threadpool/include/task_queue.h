#pragma once

#include <bits/c++config.h>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <memory>

using std::queue;
using std::mutex;
using std::condition_variable;
using std::function;
using std::shared_ptr;

using ElemType = function<void()>;

class NoCopyable
{
public:
    NoCopyable() { }
    ~NoCopyable() { }
    NoCopyable(const NoCopyable &) = delete;
    NoCopyable &operator=(const NoCopyable &) = delete;
};


class TaskQueue
    :public NoCopyable
{
public:
    TaskQueue(size_t size);

    ~TaskQueue();

    void push(ElemType);

    ElemType pop();

    bool full() const;

    bool empty() const;
private:
    std::size_t queSize_;
    queue<ElemType> que_;
    mutex mtx_;
    condition_variable notFull_;
    condition_variable notEmpty_;
};