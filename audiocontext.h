#ifndef AUDIOCONTEXT_H
#define AUDIOCONTEXT_H

#include <QAudioSink>
#include <QIODevice>
#include <QByteArray>
#include <QThread>

extern "C" {
    #include "libavformat/avformat.h"
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/avutil.h>
}

#include "audioiodevice.h"
#include "synchronizer.h"
#include "queue.h"
#include "packet.h"


class AudioContext : public QObject
{
    Q_OBJECT
public:
    AudioContext(AVFormatContext* format_context, Synchronizer* sync);

    void push_frame_to_buffer();
signals:
    void requestPacket();
    void newPacketArrived();
private:
    AVSampleFormat convert_to_AVFormat(QAudioFormat::SampleFormat format);
///////////////////////////////////////////////
///////////////////////////////////////////////
public:
    int stream_id;
    bool isMuted = false;
    qreal last_volume = 0.2;
    QAudioSink* audioSink;
    AudioIODevice* audioDevice;

    AVCodecContext* codec_context;
    Queue<Packet> packetQueue;
    QMutex decodingMutex;
private:
    SwrContext* resampleContext;

    QAudioFormat format;

    Synchronizer* sync;
};

#endif // AUDIOCONTEXT_H
