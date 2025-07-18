#pragma once

#include <atomic>
#include <bits/types/time_t.h>
#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Utils {
namespace Timer {

using TimeoutCallback = std::function<void()>;
struct TimerNode
{
    int fd;
    time_t expire;
    TimeoutCallback cb;

    TimerNode(int fd, time_t expire, TimeoutCallback &&cb): fd(fd), expire(expire), cb(cb) {}

    auto operator<(const TimerNode &rhs)->bool {
        if (expire != rhs.expire) {
            return expire < rhs.expire;
        }
        return fd < rhs.fd;
    }
};

class Timer
{
public:
    explicit Timer(size_t maxHeapSize = 60){
        heap_.reserve(maxHeapSize);
    }

    ~Timer() {}

    void tick();

    inline static time_t GetTimeStamp() {
        auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
        auto ret = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return ret.count();
    }

    void addTask(int fd, time_t timeoutMS, TimeoutCallback cb) {
        if (ref_.count(fd) == 0) {
            TimerNode node {fd, GetTimeStamp() + timeoutMS, std::move(cb)};
            heap_.emplace_back(node);
            ref_[fd] = heap_.size() - 1;
            up(heap_.size() - 1);
        }
        else {
            int idx = ref_[fd];
            heap_[idx].expire = GetTimeStamp() + timeoutMS;
            heap_[idx].cb = cb;
            if (!up(idx)) {
                down(idx);
            }
        }
    }

    void adjust(int fd, size_t timeoutMS);

    time_t getNextTick();

private:
    void down(int k);

    bool up(int k);

    void delNode(int idx);

    void swapNode(size_t l, size_t r);

private:
    std::vector<TimerNode> heap_;
    std::unordered_map<int, size_t> ref_;
};



} // namespace Timer
} // namespace Utils