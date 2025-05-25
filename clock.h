#ifndef CLOCK_H
#define CLOCK_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>

class Clock : QObject
{
    Q_OBJECT
public:
    Clock();
    void start(qint64 pts);
    void pause();
    void resume();
    void setSpeed(double newSpeed);
    qint64 get_time() const;
    void pause_check();
    void set_time(qint64 pts);
public:

private:
    QElapsedTimer timer;
    qint64 pausedTime {0};
    qint64 baseTime {0};
    double speed;
    bool paused = true;

    // QMutex playORpause_mutex;
    // QWaitCondition pauseWait;
};

#endif // CLOCK_H
