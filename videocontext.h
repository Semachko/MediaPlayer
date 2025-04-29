#ifndef VIDEOCONTEXT_H
#define VIDEOCONTEXT_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>

extern "C" {
    #include "libavformat/avformat.h"
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include "libavutil/imgutils.h"
}
#include "synchronizer.h"

class VideoContext : public QObject
{
    Q_OBJECT
public:
    VideoContext(AVFormatContext* format_context, Synchronizer* sync, QVideoSink* video_sink);

    void process(AVPacket*);
private:
    QVideoFrame decode(AVPacket*, AVFrame*);

signals:
    void frameReady(QVideoFrame frame);
public:
    int stream_id;
    AVCodecContext* codec_context;
    AVCodecParameters* codec_parameters;
    SwsContext* frame_format;
    AVFrame* rgbFrame;
    std::vector<uint8_t> buffer;

    Synchronizer* sync;
    QVideoSink* videoSink;
private:
    qint64 imagetime = 0;
    AVRational time_base;
};

#endif // VIDEOCONTEXT_H
