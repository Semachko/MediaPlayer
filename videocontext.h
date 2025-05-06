#ifndef VIDEOCONTEXT_H
#define VIDEOCONTEXT_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QPair>
#include <atomic>

extern "C" {
    #include "libavformat/avformat.h"
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include "libavutil/imgutils.h"
}
#include "synchronizer.h"
#include "frameoutput.h"
#include "queue.h"
#include "packet.h"

class VideoContext : public QObject
{
    Q_OBJECT
public:
    VideoContext(AVFormatContext* format_context, Synchronizer* sync, QVideoSink* videosink);

    void push_frame_to_queue();
signals:
    void requestPacket();
    void newPacketArrived();
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
public:
    int stream_id;
    Queue<Packet> packetQueue;
    FrameOutput* output;
    AVCodecContext* codec_context;

    QMutex decodingMutex;
private:
    QThread* outputThread;
    AVCodecParameters* codec_parameters;
    AVRational time_base;

    SwsContext* frame_format;
    std::vector<uint8_t> buffer;
    AVFrame* rgbFrame;

    Synchronizer* sync;
    QWaitCondition* imageReady;

    QVideoSink* videosink;
};

#endif // VIDEOCONTEXT_H
