#ifndef MASTERCLOCK_H
#define MASTERCLOCK_H

#include <QObject>
#include <qelapsedtimer.h>

class MasterClock : QObject
{
    Q_OBJECT
public:
    MasterClock(QObject* parent);
    void start(qint64 pts);
    void pause();
    void resume();
    void setSpeed(double newSpeed);
    qint64 getClock() const;
private:
    QElapsedTimer startTime;
    qint64 pausedTime {0};
    qint64 baseTime {0};
    double speed;
    bool paused;
};

#endif // MASTERCLOCK_H
