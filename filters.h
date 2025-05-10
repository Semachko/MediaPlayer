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
#include <QMutex>

#include "frame.h"

class Filters : public QObject
{
    Q_OBJECT
public:
    explicit Filters(AVCodecParameters*, AVRational time_base);
    Frame applyFilters(AVFrame* frame);
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
    char args[512];
    AVFilterInOut *outputs;
    AVFilterInOut *inputs;

    qreal brightness = 0.0;
    qreal contrast   = 1.0;
    qreal saturation = 1.0;

    QMutex mutex;
};

#endif // FILTERS_H
