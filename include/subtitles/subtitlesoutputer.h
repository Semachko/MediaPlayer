#ifndef SUBTITLESOUTPUTER_H
#define SUBTITLESOUTPUTER_H

#include <QObject>

#include "frame.h"
#include "media/codec.h"
#include "params/mediaparameters.h"
#include "queue.h"
#include "subtitleunit.h"
#include "sync/clock.h"
#include "sync/threadsleeper.h"

class SubtitlesOutputer : public QObject {
        Q_OBJECT
    public:
        SubtitlesOutputer(Clock* clock, MediaParameters* params);
        ~SubtitlesOutputer();
        void process_subtitles();
        void stop_and_clear();
    signals:
        void requestSubtitles();
        void outputSubtitles();
        //////////////////////////////////////////////////
    public:
        Queue<SubtitleUnit> subs_queue;

    private:
        std::mutex mutex;
        ThreadSleeper sleeper;
        Clock* const clock;
        MediaParameters* params;
};

#endif  // SUBTITLESOUTPUTER_H
