#include "clock.h"

Clock::Clock(): speed(1.0), paused(true), baseTime(0) {}

void Clock::start(qint64 pts)
{
    startTime.start();
    baseTime = pts;
}

void Clock::pause()
{
    if (!paused) {
        paused = true;
        pausedTime = startTime.elapsed();
    }
}

void Clock::resume()
{
    if (paused) {
        paused = false;
        startTime.start();
        baseTime += (pausedTime * speed);
    }
}

void Clock::setSpeed(double newSpeed)
{
    speed = newSpeed;
}

qint64 Clock::get_time() const
{
    if (paused) {
        return baseTime + (pausedTime * speed);
    } else {
        return baseTime + (startTime.elapsed() * speed);
    }
}

void Clock::set_time(qint64 ms)
{
    if (paused) {
        baseTime = ms - (pausedTime * speed);
    } else {
        baseTime = ms - (startTime.elapsed() * speed);
    }
}
