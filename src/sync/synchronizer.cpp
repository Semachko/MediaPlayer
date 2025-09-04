#include "sync/synchronizer.h"
#include <algorithm>

Synchronizer::Synchronizer(MediaParameters* params_)
    :
    params(params_),
    clock(new Clock())
{
    clock->start(0);
    if(params->isPaused)
        clock->pause();
    connect(params,&MediaParameters::speedChanged,this, [this]{clock->setSpeed(params->speed);});
    connect(params,&MediaParameters::isPausedChanged,this,&Synchronizer::play_or_pause);
}

Synchronizer::~Synchronizer()
{
    delete clock;
}

void Synchronizer::play_or_pause()
{
    std::lock_guard lock(pause_mutex);
    if (params->isPaused)
        clock->pause();
    else{
        pauseWait.notify_all();
        clock->resume();
    }
}

void Synchronizer::check_pause()
{
    std::unique_lock lock(pause_mutex);
    while(params->isPaused)
        pauseWait.wait(lock);
}

qint64 Synchronizer::get_time()
{
    std::unique_lock lock(time_mutex);
    return std::min(clock->get_time(), params->file->globalTime.load());
}


