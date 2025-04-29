#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include "masterclock.h"

class Synchronizer : public QObject
{
    Q_OBJECT
public:
    Synchronizer(MasterClock* clock, QObject* parent = nullptr);
    void playORpause();
public:
    MasterClock* clock;

    QMutex playORpause_mutex;
    QWaitCondition pauseWait;
    bool isPaused = true;
};

#endif // SYNCHRONIZER_H
