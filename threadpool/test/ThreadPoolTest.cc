#include "thread_pool.h"
#include <functional>
#include <iostream>
#include <memory>
#include <unistd.h>

void task()
{
    std::cout << "This is Thread: " << gettid() << '\n';
}

int main()
{
    ThreadPool pool(4, 10);
    pool.start();

    pool.addTask(task);
    pool.addTask(task);
    pool.addTask(task);
    pool.addTask(task);

    pool.stop();
    return 0;
}