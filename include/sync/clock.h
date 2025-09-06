#ifndef CLOCK_H
#define CLOCK_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>

#include "media/mediaparameters.h"

class Clock
{
public:
    Clock(MediaParameters* params);
    void pause();
    void resume();
    qint64 get_time();
    void set_speed(qreal speed);
    void set_time(qint64 pts);
public:

private:
    MediaParameters* params;
    QElapsedTimer timer;
    qint64 current_time {0};
    qreal speed;
};

#endif // CLOCK_H
