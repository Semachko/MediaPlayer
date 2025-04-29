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

class AudioContext : public QObject
{
    Q_OBJECT
public:
    AudioContext(AVFormatContext* format_context);

    void process(AVPacket*);

public:
    int stream_id;
private:
    AVCodecContext* codec_context;
    AVCodecParameters* codec_parameters;
    SwrContext* resampleContext;

    QAudioFormat format;
    QAudioSink* audioSink;
    AudioIODevice* audioDevice;
};

#endif // AUDIOCONTEXT_H
