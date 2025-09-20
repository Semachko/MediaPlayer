#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H

#include <QObject>
#include <QElapsedTimer>

#include "media/mediaparameters.h"
#include "sync/iclock.h"

class RealTimeClock : public QObject, public IClock
{ Q_OBJECT
public:
    RealTimeClock(MediaParameters* params);
    void pause();
    void resume();
    qreal get_time() override;
    void set_time(qreal seconds) override;
    void set_speed(qreal speed);
private:
    MediaParameters* params;
    QElapsedTimer timer;
    qreal paused_time {0};
    qreal base_time {0};
    qreal speed;
};

#endif // REALTIMECLOCK_H
