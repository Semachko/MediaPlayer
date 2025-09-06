#include "sync/clock.h"
#include <QDebug>

Clock::Clock(MediaParameters* params_)
    : params(params_), speed(params_->speed)
{
    timer.start();
}
void Clock::pause()
{
    current_time += timer.elapsed() * speed;
}
void Clock::resume()
{
    timer.start();
}
void Clock::set_speed(qreal new_speed)
{
    if (!params->isPaused)
        current_time += timer.restart() * speed;
    speed = new_speed;
}
qint64 Clock::get_time()
{
    if (params->isPaused)
        return current_time;
    current_time += timer.restart() * speed;
    return current_time;
}
void Clock::set_time(qint64 ms)
{
    current_time = ms;
    timer.start();
}
