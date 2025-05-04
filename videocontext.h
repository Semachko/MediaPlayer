#ifndef VIDEOCONTEXT_H
#define VIDEOCONTEXT_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QPair>

extern "C" {
    #include "libavformat/avformat.h"
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include "libavutil/imgutils.h"
}
#include "synchronizer.h"
#include "frameoutput.h"

class VideoContext : public QObject
{
    Q_OBJECT
public:
    VideoContext(AVFormatContext* format_context, Synchronizer* sync);

    void output_image();
    void fill_frameQueue();
    void push_frame_to_queue();
signals:
    void newPacketReady();
    void imageToOutput(QVideoFrame frame);
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
public:
    int stream_id;
    int queueSize = 20;
    QMutex queueMutex;
    std::queue<AVPacket*> packetQueue;
    FrameOutput* output;

private:
    QThread* outputThread;
    AVCodecContext* codec_context;
    AVCodecParameters* codec_parameters;
    AVRational time_base;

    SwsContext* frame_format;
    std::vector<uint8_t> buffer;
    AVFrame* rgbFrame;

    Synchronizer* sync;
    QMutex imageMutex;
    QWaitCondition* imageReady;
};

#endif // VIDEOCONTEXT_H
