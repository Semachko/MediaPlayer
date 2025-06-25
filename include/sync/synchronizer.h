#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <queue>

#include "sync/clock.h"

class Synchronizer : public QObject
{
    Q_OBJECT
public:
    Synchronizer();
    ~Synchronizer();

    void play_or_pause();
    void check_pause();
    qint64 get_time();
public:
    bool isPaused = true;
    Clock* clock;
private:
    QMutex pauseMutex;
    QWaitCondition pauseWait;
};

#endif // SYNCHRONIZER_H
