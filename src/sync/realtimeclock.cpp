#include "sync/realtimeclock.h"
#include <QDebug>

RealTimeClock::RealTimeClock(MediaParameters* params_)
    : params(params_), speed(params_->speed)
{
    timer.start();
    if(params->isPaused)
        pause();
    connect(params,&MediaParameters::speedChanged,this, [this]{set_speed(params->speed);});
    connect(params,&MediaParameters::isPausedChanged,this,[this]{
        if (params->isPaused) pause();
        else resume();
    });
}
void RealTimeClock::pause()
{
    paused_time = (timer.nsecsElapsed() / 1e9) * speed;
}
void RealTimeClock::resume()
{
    timer.start();
    base_time += paused_time;
    paused_time = 0;
}

void RealTimeClock::set_speed(qreal new_speed)
{
    if (params->isPaused) {
        base_time += paused_time;
        paused_time = 0;
    } else {
        base_time += (timer.nsecsElapsed() / 1e9) * speed;
        timer.start();
    }
    speed = new_speed;
}

qreal RealTimeClock::get_time()
{
    if (params->isPaused)
        return base_time + paused_time;
    else
        return base_time + (timer.nsecsElapsed() / 1e9) * speed;
}
void RealTimeClock::set_time(qreal seconds)
{
    if (params->isPaused)
        base_time = seconds - paused_time;
    else
        base_time = seconds - (timer.nsecsElapsed() / 1e9) * speed;
    if (base_time < 0)
        base_time = 0;
}
