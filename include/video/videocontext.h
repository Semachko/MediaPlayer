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
#include "sync/synchronizer.h"
#include "video/frameoutput.h"
#include "video/filters.h"
#include "video/imageconverter.h"

class VideoContext : public IMediaContext
{
    Q_OBJECT
public:
    VideoContext(QVideoSink* videosink, AVStream* stream, Synchronizer* sync, qreal bufferization_time);
    ~VideoContext();

    void process_packet() override;
    qint64 buffer_available() override;
    void set_brightness(qreal value);
    void set_contrast(qreal value);
    void set_saturation(qreal value);

    void decode(Packet& packet);
    void filter_and_output();
private:
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
public:
    FrameOutput* output;
    Decoder decoder;

    std::mutex mutex;
    QVideoSink* videosink;
private:
    QThread* outputThread;

    // Filters* filters;
    // ImageConverter* converter;
    Synchronizer* sync;
    QWaitCondition* imageReady;
    qint64 maxBufferSize;
};

#endif // VIDEOCONTEXT_H
