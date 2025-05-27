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

#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
}

#include "imediacontext.h"
#include "audioiodevice.h"
#include "synchronizer.h"
#include "queue.h"
#include "packet.h"
#include "equalizer.h"
#include "sampleconverter.h"

class AudioContext : public IMediaContext
{
    Q_OBJECT
public:
    AudioContext(AVFormatContext* format_context, Synchronizer* sync, qreal bufferization_time);
    ~AudioContext();

    void decode_and_output() override;
    qint64 buffer_available() override;
    void set_low(qreal value);
    void set_mid(qreal value);
    void set_high(qreal value);
    void set_speed(qreal speed);

    void decode();
    void equalizer_and_output();
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
    QAudioFormat format;

    AVCodecContext* codec_context;
    QMutex audioMutex;
private:
    SampleConverter* converter;
    Equalizer* equalizer;

    Synchronizer* sync;
    qint64 maxBufferSize;
    qreal bufferization_time;
};

#endif // AUDIOCONTEXT_H
