#include "sync/synchronizer.h"
#include <algorithm>

Synchronizer::Synchronizer(IClock* clock_) : clock(clock_)
{
}

Synchronizer::~Synchronizer()
{
    delete clock;
}
void Synchronizer::set_time(qreal seconds)
{
    std::lock_guard lock(timer_mutex);
    clock->set_time(seconds);
}
qreal Synchronizer::get_time()
{
    std::lock_guard lock(timer_mutex);
    return clock->get_time();
}


