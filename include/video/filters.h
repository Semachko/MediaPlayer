#ifndef FILTERS_H
#define FILTERS_H

#define __STDC_CONSTANT_MACROS

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include "libavformat/avformat.h"
}

#include <QObject>
#include <mutex>

#include "frame.h"
#include "media/codec.h"
#include "media/mediaparameters.h"

class Filters : public QObject
{Q_OBJECT
public:
    explicit Filters(const Codec&, VideoParameters*);
    ~Filters();

    Frame applyFilters(Frame frame);
    void set_brightness(qreal value);
    void set_contrast(qreal value);
    void set_saturation(qreal value);
private:
    void update_filters();
private:
    AVFilterGraph* filter_graph = nullptr;
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;
    const AVFilter *buffersrc;
    const AVFilter *buffersink;
    std::string args;
    AVFilterInOut *outputs;
    AVFilterInOut *inputs;

    VideoParameters* params;
    std::mutex mutex;
};

#endif // FILTERS_H
