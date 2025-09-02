#include "sync/clock.h"
#include <QDebug>

Clock::Clock(): speed(1.0), paused(true), baseTime(0) {}

void Clock::start(qint64 pts)
{
    timer.start();
    baseTime = pts;
}

void Clock::pause()
{
    if (!paused) {
        paused = true;
        pausedTime = timer.elapsed();
    }
}

void Clock::resume()
{
    if (paused) {
        paused = false;
        timer.start();
        baseTime += (pausedTime * speed);
    }
}

void Clock::setSpeed(double newSpeed)
{
    if (paused) {
        baseTime += (pausedTime * speed);
        pausedTime = 0;
    } else {
        baseTime += (timer.elapsed() * speed);
        timer.start();
    }
    speed = newSpeed;
}


qint64 Clock::get_time() const
{
    if (paused)
        return baseTime + (pausedTime * speed);
    else
        return baseTime + (timer.elapsed() * speed);
}

void Clock::set_time(qint64 ms)
{
    if (paused) {
        baseTime = ms - (pausedTime * speed);
    } else {
        baseTime = ms - (timer.elapsed() * speed);
    }
}
