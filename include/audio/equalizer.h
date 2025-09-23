#ifndef EQUALIZER_H
#define EQUALIZER_H

#define __STDC_CONSTANT_MACROS

extern "C" {
#include <libavutil/channel_layout.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavcodec/avcodec.h>
}

#include <mutex>
#include <QObject>
#include <QAudioFormat>

#include "frame.h"
#include "media/codec.h"
#include "media/mediaparameters.h"

class Equalizer : public QObject
{Q_OBJECT
public:
    explicit Equalizer(Codec& codec, MediaParameters* params_);
    ~Equalizer();
    Frame applyEqualizer(Frame frame);
    void update_equalizer();
private:
    AVFilterGraph* filter_graph = nullptr;
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;
    const AVFilter *buffersrc;
    const AVFilter *buffersink;
    std::string args;
    AVFilterInOut *outputs;
    AVFilterInOut *inputs;

    Codec& codec;
    MediaParameters* params;
    std::mutex mutex;
};

#endif // EQUALIZER_H
