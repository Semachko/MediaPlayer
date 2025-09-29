#ifndef VIDEOCONTEXT_H
#define VIDEOCONTEXT_H

extern "C" {
#include "libavformat/avformat.h"
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include "libavutil/imgutils.h"
}

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QPair>

#include "media/codec.h"
#include "media/decoder.h"
#include "media/imediacontext.h"
#include "media/mediaparameters.h"
#include "sync/clock.h"
#include "video/frameoutput.h"
#include "video/filters.h"
#include "video/imageconverter.h"

class VideoContext : public IMediaContext
{
public:
    VideoContext(AVStream* stream, Clock* clock, MediaParameters* params, qreal bufferization_time);
    ~VideoContext();

    void process_packet() override;
    qint64 buffer_available() override;
    void clear() override;
//////////////////////////////////////////////////////////
public:
    std::mutex mutex;
    FrameOutput* outputer;
private:
    QThread* outputThread;
    Clock* clock;
    qint64 maxBufferSize;
};

#endif // VIDEOCONTEXT_H
