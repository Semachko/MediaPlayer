#ifndef SUBTITLESOUTPUTER_H
#define SUBTITLESOUTPUTER_H

#include <QObject>
#include "media/codec.h"
#include "media/mediaparameters.h"
#include "sync/clock.h"
#include "sync/threadsleeper.h"
#include "queue.h"
#include "frame.h"
#include "subtitle.h"

class SubtitlesOutputer : public QObject
{
    Q_OBJECT
public:
    SubtitlesOutputer(Clock* clock, Codec& codec_, MediaParameters* params);
    ~SubtitlesOutputer();
    void process_subtitles();
    void stop_and_clear();
signals:
    void requestSubtitles();
    void outputSubtitles();
//////////////////////////////////////////////////
public:
    Queue<Subtitle> subs_queue;
private:
    std::mutex mutex;
    ThreadSleeper sleeper;
    const Codec& codec;
    Clock* const clock;
    MediaParameters* const params;
};

#endif // SUBTITLESOUTPUTER_H
