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

#include "imediacontext.h"
#include "synchronizer.h"
#include "frameoutput.h"
#include "filters.h"
#include "imageconverter.h"

class VideoContext : public IMediaContext
{
    Q_OBJECT
public:
    VideoContext(QVideoSink* videosink, AVFormatContext* format_context, Synchronizer* sync, int stream_id, qreal bufferization_time);
    ~VideoContext();

    void decode_and_output() override;
    qint64 buffer_available() override;
    void set_brightness(qreal value);
    void set_contrast(qreal value);
    void set_saturation(qreal value);

    void decode();
    void filter_and_output();
private:
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
public:
    int stream_id;
    FrameOutput* output;
    AVCodecContext* codec_context;

    QMutex videoMutex;
    QVideoSink* videosink;
private:
    QThread* outputThread;

    Filters* filters;
    ImageConverter* converter;
    Synchronizer* sync;
    QWaitCondition* imageReady;
    qint64 maxBufferSize;
};

#endif // VIDEOCONTEXT_H
