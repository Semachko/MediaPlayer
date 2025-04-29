#include "synchronizer.h"

Synchronizer::Synchronizer(MasterClock *clock, QObject * parent)
    : QObject(parent),
    clock(clock)
    {}

void Synchronizer::playORpause()
{
    QMutexLocker locker(&playORpause_mutex);
    isPaused = !isPaused;
    if (!isPaused){
        pauseWait.wakeAll();
        clock->resume();
    }else
        clock->pause();
}
