#ifndef SUBTITLECONTEXT_H
#define SUBTITLECONTEXT_H

#include <QObject>
#include <QThread>
#include "media/imediacontext.h"
#include "media/mediaparameters.h"
#include "subtitles/subtitlesoutputer.h"
#include "sync/clock.h"

class SubtitleContext : public IMediaContext
{
public:
    SubtitleContext(AVStream* stream, Clock* clock, MediaParameters* params);
    ~SubtitleContext();
    void process_packet() override;
    qint64 buffer_available() override;
    void clear() override;
//////////////////////////////////////////////////////////
private:
    std::mutex mutex;
    Clock* clock;
    QThread* outputerThread;
    SubtitlesOutputer* outputer;
};

#endif // SUBTITLECONTEXT_H
