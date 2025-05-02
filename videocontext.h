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

    void process(AVPacket*);
    void start_output();

    void output_image();
    void fill_frameQueue();
    void push_frame_to_queue();
private:
    void synchronize();
signals:
    void newPacketReady();
    void imageToOutput(QVideoFrame frame);
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
public:
    int stream_id;
    std::queue<AVPacket*> packetQueue;
    int queueSize = 5;
private:
    AVCodecContext* codec_context;
    AVCodecParameters* codec_parameters;
    AVRational time_base;

    SwsContext* frame_format;
    std::vector<uint8_t> buffer;
    AVFrame* rgbFrame;

    Synchronizer* sync;
    QMutex imageMutex;
    QWaitCondition* imageReady;

    QThread* outputThread;
public:
    FrameOutput* output;
};

#endif // VIDEOCONTEXT_H
