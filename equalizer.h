#ifndef EQUALIZER_H
#define EQUALIZER_H

#define __STDC_CONSTANT_MACROS

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
}

#include <QMutex>
#include <QAudioFormat>
#include "frame.h"

class Equalizer
{
public:
    //explicit Equalizer(QAudioFormat&,AVSampleFormat sampleFormat);
    explicit Equalizer(AVCodecContext* codec_context);
    Frame applyEqualizer(AVFrame* frame);
    void set_low(qreal value);
    void set_mid(qreal value);
    void set_high(qreal value);
private:
    void update_equalizer();
private:
    AVCodecContext* codec_context;
    AVFilterGraph* filter_graph = nullptr;
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;
    const AVFilter *buffersrc;
    const AVFilter *buffersink;
    std::string args;
    AVFilterInOut *outputs;
    AVFilterInOut *inputs;

    qreal low  = 0.0;
    qreal mid  = 0.0;
    qreal high = 0.0;

    QMutex mutex;
};

#endif // EQUALIZER_H
