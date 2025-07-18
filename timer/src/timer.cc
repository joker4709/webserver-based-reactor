#include "timer.h"
#include <functional>


namespace Utils {
namespace Timer {

void Timer::tick()
{
    while (!heap_.empty()) {
        if (heap_[0].expire <= GetTimeStamp()) {
            heap_[0].cb();
            delNode(0);
        }
        else {
            break;
        }
    }
}

void Timer::down(int k) {
    int t = k;
    int l = 2 * t + 1;
    int r = 2 * t + 2;
    if (l < heap_.size() && heap_[l] < heap_[t]) t = l;
    if (r < heap_.size() && heap_[r] < heap_[t]) t = r;
    if (t != k) {
        swapNode(t, k);
        down(t);
    }
}

bool Timer::up(int k)
{
    int temp = k;
    while ((k - 1) / 2 && heap_[k] < heap_[(k - 1) / 2]) {
        swapNode((k - 1) / 2, k);
        k = (k - 1) / 2;
    }
    return temp != k;
}

void Timer::delNode(int idx) {
    if (idx >= heap_.size() || idx < 0) {
        throw "idx is not valid";
        return;
    }
    int last = heap_.size() - 1;
    swapNode(idx, last);
    ref_.erase(heap_.back().fd);
    heap_.pop_back();
    down(idx);
    up(idx);
}

void Timer::swapNode(size_t l, size_t r)
{
    if (l < 0 || l >= heap_.size() || r < 0 || r >= heap_.size()) return;
    std::swap(heap_[l], heap_[r]);
    ref_[heap_[l].fd] = l;
    ref_[heap_[r].fd] = r;
}

void Timer::adjust(int fd, size_t timeoutMS)
{
    if (ref_.count(fd) == 0) {
        return;
    }
    else {
        int idx = ref_[fd];
        heap_[idx].expire = GetTimeStamp() + timeoutMS;
        if (up(idx)) {
            down(idx);
        }
    }
}

time_t Timer::getNextTick()
{
    tick();
    if (heap_.empty()) {
        return 0;
    }
    else {
        return heap_[0].expire - GetTimeStamp();
    }
}
} // namespace Timer
} // namespace Utils