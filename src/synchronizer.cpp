#include "sync/synchronizer.h"

Synchronizer::Synchronizer()
    :
    clock(new Clock())
{
    clock->start(0);
}

Synchronizer::~Synchronizer()
{
    delete clock;
}

void Synchronizer::play_or_pause()
{
    QMutexLocker locker(&pauseMutex);
    isPaused = !isPaused;
    if (!isPaused){
        pauseWait.wakeAll();
        clock->resume();
    }else
        clock->pause();
}

void Synchronizer::check_pause()
{
    QMutexLocker locker(&pauseMutex);
    while(isPaused)
        pauseWait.wait(&pauseMutex);
}

qint64 Synchronizer::get_time()
{
    return clock->get_time();
}


