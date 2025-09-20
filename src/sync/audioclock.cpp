#include "sync/audioclock.h"
#include <QDebug>

AudioClock::AudioClock()
{
}

qreal AudioClock::get_time()
{
    return audiotime;
}

void AudioClock::set_time(qreal seconds)
{
    audiotime = seconds;
}
