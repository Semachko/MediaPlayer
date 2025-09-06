#include "sync/synchronizer.h"
#include <algorithm>

Synchronizer::Synchronizer(MediaParameters* params_)
    :
    params(params_),
    clock(params_)
{
    connect(params,&MediaParameters::speedChanged,this, [this]{clock.set_speed(params->speed);});
    connect(params,&MediaParameters::isPausedChanged,this,&Synchronizer::play_or_pause);
}

void Synchronizer::play_or_pause()
{
    std::lock_guard lock(timer_mutex);
    if (params->isPaused){
        clock.pause();
    }
    else{
        pauseWait.notify_all();
        clock.resume();
    }
}
void Synchronizer::check_pause()
{
    std::unique_lock lock(timer_mutex);
    while(params->isPaused)
        pauseWait.wait(lock);
}
void Synchronizer::set_time(qint64 time_ms)
{
    std::lock_guard lock(timer_mutex);
    clock.set_time(time_ms);
}
qint64 Synchronizer::get_time()
{
    std::lock_guard lock(timer_mutex);
    return std::min(clock.get_time(), params->file->globalTime.load());
}


