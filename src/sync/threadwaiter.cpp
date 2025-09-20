#include "sync/threadwaiter.h"

ThreadWaiter::ThreadWaiter() {}

void ThreadWaiter::wait(qreal seconds)
{
    auto wait_time = std::chrono::duration<double>(seconds);
    std::unique_lock locker(mutex);
    waked = false;
    cv.wait_for(locker, wait_time,[this]{return waked;});
}

void ThreadWaiter::wake()
{
    std::lock_guard _(mutex);
    waked = true;
    cv.notify_all();
}
