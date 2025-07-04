﻿#ifndef AUDIOCONTEXT_H
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

#include "media/imediacontext.h"
#include "audio/audioiodevice.h"
#include "sync/synchronizer.h"
#include "audio/equalizer.h"
#include "audio/sampleconverter.h"
#include "queue.h"
#include "packet.h"

class AudioContext : public IMediaContext
{
    Q_OBJECT
public:
    AudioContext(AVFormatContext* format_context, Synchronizer* sync, int stream_id, qreal bufferization_time);
    ~AudioContext();

    void decode_and_output() override;
    qint64 buffer_available() override;
    void set_low(qreal value);
    void set_mid(qreal value);
    void set_high(qreal value);
    void set_speed(qreal speed);
    void set_volume(qreal volume);

    void decode(Packet& packet);
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
    const qint64 MIN_BUFFER_SIZE = 4092;
    //qreal bufferization_time;

};

#endif // AUDIOCONTEXT_H
