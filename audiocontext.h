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

class AudioContext : public QObject
{
    Q_OBJECT
public:
    AudioContext(AVFormatContext* format_context);

    void process(AVPacket*);

public:
    int stream_id;
    bool isMuted = false;
    qreal last_volume = 0.2;
    QAudioSink* audioSink;

    std::queue<AVPacket*> audioPacketQueue;
    std::queue<AVFrame*> audioFrameQueue;
private:
    AVCodecContext* codec_context;
    AVCodecParameters* codec_parameters;
    SwrContext* resampleContext;

    QAudioFormat format;
    AudioIODevice* audioDevice;


};

#endif // AUDIOCONTEXT_H
