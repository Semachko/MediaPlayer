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
    ~Synchronizer();

    void play_or_pause();
    void check_pause();
    qint64 get_time();
public:
    Clock* clock;
private:
    MediaParameters* params;
    std::mutex pause_mutex;
    std::mutex time_mutex;
    std::condition_variable pauseWait;
};

#endif // SYNCHRONIZER_H
