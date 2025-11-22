#ifndef AUDIOOUTPUTER_H
#define AUDIOOUTPUTER_H

#include <QAudioSink>
#include <QByteArray>
#include <QIODevice>
#include <QThread>
#include <chrono>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>

#include "libavformat/avformat.h"
}

#include "audio/audiooutputer.h"
#include "audio/equalizer.h"
#include "audio/sampleconverter.h"
#include "media/codec.h"
#include "media/imediacontext.h"
#include "packet.h"
#include "queue.h"
#include "sync/clock.h"

class AudioContext : public IMediaContext {
    public:
        AudioContext(AVStream* stream, Clock* clock, MediaParameters* params, qreal bufferization_time);
        ~AudioContext();

        void process_packet() override;
        qint64 buffer_available() override;
        void clear() override;
        void push_to_outputer(QQueue<Frame>&);

        void mute_unmute();
        void set_volume();
        void pause_changed();

    private:
        AVSampleFormat convert_to_AVFormat(QAudioFormat::SampleFormat format);
        ///////////////////////////////////////////////
        ///////////////////////////////////////////////
    public:
    private:
        QAudioSink* audioSink;
        AudioOutputer* outputer;
        MediaParameters* params;
        std::mutex mutex;
        QThread* outputThread;
        SampleFormat outputFormat;
        QAudioFormat format;
        Clock* clock;
        qint64 bytes_per_sec;
        qint64 MAX_BUFFER_SIZE;
        qreal bufferization_time;
        qreal last_volume;
};

#endif  // AUDIOOUTPUTER_H
