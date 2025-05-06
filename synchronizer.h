#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

extern "C" {
    #include "libavutil/frame.h"
    #include "libavcodec/packet.h"
}

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <queue>

#include "clock.h"

class Synchronizer : public QObject
{
    Q_OBJECT
public:
    Synchronizer(QObject* parent = nullptr);

    void play_or_pause();
    void check_pause();
    qint64 get_time();
public:
    bool isPaused = true;
    Clock* clock;
private:
    QMutex pauseMutex;
    QWaitCondition pauseWait;
};

#endif // SYNCHRONIZER_H
