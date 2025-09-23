#include "sync/clock.h"
#include <algorithm>
#include <QThread>

Clock::Clock(MediaParameters* params_)
    : params(params_), speed(params_->speed)
{
    timer.start();
    if(params->isPaused)
        pause();
    connect(params,&MediaParameters::speedChanged,this, [this]{set_speed(params->speed);}, Qt::QueuedConnection);
    connect(params,&MediaParameters::isPausedChanged,this,[this]{
        if (params->isPaused) pause();
        else resume();
    }, Qt::QueuedConnection);
}
qreal Clock::get_time()
{
    std::lock_guard _(mutex);
    if (params->isPaused)
        return std::min(current_time, params->file->globalTime.load());
    qreal seconds = timer.nsecsElapsed() / 1'000'000'000.0;
    current_time += seconds * speed;
    timer.start();
    return std::min(current_time, params->file->globalTime.load());
}
void Clock::set_time(qreal secs)
{
    std::lock_guard _(mutex);
    current_time = secs;
    timer.start();
}

void Clock::pause()
{
    std::lock_guard _(mutex);
    qreal seconds = timer.nsecsElapsed() / 1'000'000'000.0;
    current_time += seconds * speed;
}
void Clock::resume()
{
    std::lock_guard _(mutex);
    timer.start();
}
void Clock::set_speed(qreal new_speed)
{
    std::lock_guard _(mutex);
    if (!params->isPaused){
        qreal seconds = timer.nsecsElapsed() / 1'000'000'000.0;
        current_time += seconds * speed;
        timer.start();
    }
    speed = new_speed;
}

