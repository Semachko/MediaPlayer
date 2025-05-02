#ifndef MASTERCLOCK_H
#define MASTERCLOCK_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>

class Clock : QObject
{
    Q_OBJECT
public:
    Clock(QObject* parent);
    void start(qint64 pts);
    void pause();
    void resume();
    void setSpeed(double newSpeed);
    qint64 get_time() const;
    void pause_check();
public:

private:
    QElapsedTimer startTime;
    qint64 pausedTime {0};
    qint64 baseTime {0};
    double speed;
    bool paused;

    QMutex playORpause_mutex;
    QWaitCondition pauseWait;
    bool isPaused = true;
};

#endif // MASTERCLOCK_H
