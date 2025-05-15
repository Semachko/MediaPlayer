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
    VideoContext(AVFormatContext* format_context, Synchronizer* sync, QVideoSink* videosink);

    void push_frame_to_buffer() override;
    void set_brightness(qreal value);
    void set_contrast(qreal value);
    void set_saturation(qreal value);
private:
    void convert_and_push_images();
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
public:
    int stream_id;
    FrameOutput* output;
    AVCodecContext* codec_context;

    QMutex decodingMutex;
private:
    QThread* outputThread;
    AVCodecParameters* codec_parameters;
    AVRational time_base;

    Filters* filters;
    ImageConverter* converter;
    Synchronizer* sync;
    QWaitCondition* imageReady;

    QVideoSink* videosink;
};

#endif // VIDEOCONTEXT_H
