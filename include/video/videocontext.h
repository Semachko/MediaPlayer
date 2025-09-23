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
//Q_OBJECT
public:
    VideoContext(AVStream* stream, Clock* sync, MediaParameters* params, qreal bufferization_time);
    ~VideoContext();

    void process_packet() override;
    qint64 buffer_available() override;
    void get_and_output_frames();
    void clear();
private:
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
public:
    std::mutex mutex;
    Decoder decoder;
    FrameOutput* output;
private:
    QThread* outputThread;
    Clock* clock;
    qint64 maxBufferSize;
};

#endif // VIDEOCONTEXT_H
