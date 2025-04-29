#include "synchronizer.h"

Synchronizer::Synchronizer(QObject * parent)
    : QObject(parent),
    clock(new MasterClock(this))
{
    clock->start(0);
}

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
