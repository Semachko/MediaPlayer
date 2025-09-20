#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>
#include <mutex>

#include "sync/realtimeclock.h"
#include "media/mediaparameters.h"

class Synchronizer
{
public:
    Synchronizer(IClock* clock_);
    ~Synchronizer();
    void set_time(qreal);
    qreal get_time();
private:
    IClock* clock;
    std::mutex timer_mutex;
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
