#ifndef AUDIOCLOCK_H
#define AUDIOCLOCK_H

#include <QObject>
#include "sync/iclock.h"

class AudioClock : public IClock
{
public:
    AudioClock();
    qreal get_time() override;
    void set_time(qreal seconds) override;
private:
    qreal audiotime{0.0};
};

#endif // AUDIOCLOCK_H
