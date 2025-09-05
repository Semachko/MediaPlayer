#ifndef AUDIOOUTPUTER_H
#define AUDIOOUTPUTER_H

#include <QAudioSink>
#include <QIODevice>
#include <QByteArray>
#include <QThread>
#include <chrono>
#include <mutex>

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

#include "media/codec.h"
#include "media/imediacontext.h"
#include "audio/audiooutputer.h"
#include "sync/synchronizer.h"
#include "audio/equalizer.h"
#include "audio/sampleconverter.h"
#include "queue.h"
#include "packet.h"

class AudioContext : public IMediaContext
{
//Q_OBJECT
public:
    AudioContext(AVStream* stream,  Synchronizer* sync, MediaParameters* params, qreal bufferization_time);
    ~AudioContext();

    void process_packet() override;
    void decode_packet(Packet& packet);
    void get_and_output_samples();
    qint64 buffer_available() override;

    void mute_unmute();
    void set_volume();
    void pause_changed();
private:
    AVSampleFormat convert_to_AVFormat(QAudioFormat::SampleFormat format);
///////////////////////////////////////////////
///////////////////////////////////////////////
public:
    QAudioSink* audioSink;
    AudioOutputer* audio_outputer;
    MediaParameters* params;
    QAudioFormat format;
    std::mutex mutex;
private:
    QThread* outputThread;

    Synchronizer* sync;
    qint64 maxBufferSize;
    qreal last_volume = 0.2;
};

#endif // AUDIOOUTPUTER_H
