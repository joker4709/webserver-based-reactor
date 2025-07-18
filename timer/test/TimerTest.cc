#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "timer.h"




int main ()
{
    Utils::Timer::Timer timer;

    timer.addTask(1, 1000, [](){
        std::cout << __func__ << ": " << __LINE__ << '\n';
    });

    timer.addTask(2, 2000, [](){
        std::cout << __func__ << ": " << __LINE__ << '\n';
    });

    timer.addTask(3, 3000, [](){
        std::cout << __func__ << ": " << __LINE__ << '\n';
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::cout << timer.getNextTick() << '\n';
    
    return 0;
}