#ifndef AUDIOCONTEXT_H
#define AUDIOCONTEXT_H

#include <QAudioSink>
#include <QIODevice>
#include <QByteArray>
#include <QThread>
#include <queue>

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

class AudioContext : public QObject
{
    Q_OBJECT
public:
    AudioContext(AVFormatContext* format_context, Synchronizer* sync);

    void process(AVPacket*);
    void push_frame_to_buffer();
signals:
    void newPacketReady();
private:
    AVSampleFormat convert_to_AVFormat(QAudioFormat::SampleFormat format);
signals:
    void packetDecoded();
///////////////////////////////////////////////
///////////////////////////////////////////////
public:
    int stream_id;
    bool isMuted = false;
    qreal last_volume = 0.2;
    QAudioSink* audioSink;

    int queueSize = 16;
    QMutex queueMutex;
    std::queue<AVPacket*> packetQueue;

private:
    AVCodecContext* codec_context;
    AVCodecParameters* codec_parameters;
    SwrContext* resampleContext;

    QAudioFormat format;
    AudioIODevice* audioDevice;

    Synchronizer* sync;
};

#endif // AUDIOCONTEXT_H
