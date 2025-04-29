#include "masterclock.h"

MasterClock::MasterClock(QObject *parent): QObject(parent), speed(1.0), paused(true), baseTime(0) {}

void MasterClock::start(qint64 pts)
{
    startTime.start();
    baseTime = pts;
}

void MasterClock::pause()
{
    if (!paused) {
        paused = true;
        pausedTime = startTime.elapsed();
    }
}

void MasterClock::resume()
{
    if (paused) {
        paused = false;
        startTime.start();
        baseTime += (pausedTime * speed);
    }
}

void MasterClock::setSpeed(double newSpeed)
{
    speed = newSpeed;
}

qint64 MasterClock::getClock() const
{
    if (paused) {
        return baseTime + (pausedTime * speed);
    } else {
        return baseTime + (startTime.elapsed() * speed);
    }
}
