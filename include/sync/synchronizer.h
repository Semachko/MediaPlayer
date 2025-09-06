#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>
#include <mutex>

#include "sync/clock.h"
#include "media/mediaparameters.h"

class Synchronizer : public QObject
{
    Q_OBJECT
public:
    Synchronizer(MediaParameters* params);

    void play_or_pause();
    void check_pause();
    void set_time(qint64);
    qint64 get_time();
public:
    Clock clock;
private:
    MediaParameters* params;
    std::mutex timer_mutex;
    std::condition_variable pauseWait;
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
