#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>
#include <QElapsedTimer>
#include <mutex>

#include "media/mediaparameters.h"

class Clock : public QObject
{   Q_OBJECT
public:
    Clock(MediaParameters* params);
    void set_time(qreal);
    qreal get_time();
    void pause();
    void resume();
    void set_speed(qreal);
private:
    std::mutex mutex;
    qreal speed;
    qreal current_time = 0;
    MediaParameters* params;
    QElapsedTimer timer;
};


class ScopedPause{
public:
    ScopedPause(MediaParameters* params_) : params(params_){
        if (!params->isPaused)
            params->setIsPaused(true);
    }
    ~ScopedPause(){
        if (params->isPaused)
            params->setIsPaused(false);
    }
private:
    MediaParameters* params;
};

#endif // SYNCHRONIZER_H
