#include "filters.h"
#include <QDebug>

Filters::Filters(AVCodecParameters* codec_parameters, AVRational time_base)
{
    args =
        "video_size="+std::to_string(codec_parameters->width)+"x"+std::to_string(codec_parameters->height)
        +":pix_fmt="+std::to_string(codec_parameters->format)
        +":time_base="+std::to_string(time_base.num)+"/"+std::to_string(time_base.den);

    update_filters();
}

Frame Filters::applyFilters(Frame frame)
{
    QMutexLocker _(&mutex);
    Frame filtered_frame = make_shared_frame();
    av_buffersrc_add_frame(buffersrc_ctx, frame.get());
    av_buffersink_get_frame(buffersink_ctx, filtered_frame.get());
    return filtered_frame;
}

void Filters::set_brightness(qreal value)
{
    brightness = value;
    update_filters();
}

void Filters::set_contrast(qreal value)
{
    contrast = value;
    update_filters();
}

void Filters::set_saturation(qreal value)
{
    saturation = value;
    update_filters();
}

void Filters::update_filters()
{
    QMutexLocker _(&mutex);

    if (filter_graph) {
        avfilter_graph_free(&filter_graph);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
    }

    filter_graph = avfilter_graph_alloc();

    buffersrc  = avfilter_get_by_name("buffer");
    buffersink = avfilter_get_by_name("buffersink");
    avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args.c_str(), nullptr, filter_graph);
    avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", nullptr, nullptr, filter_graph);

    outputs = avfilter_inout_alloc();
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = nullptr;

    inputs = avfilter_inout_alloc();
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = nullptr;

    std::string temp
        = "eq=brightness="+std::to_string(brightness)
          + ":contrast="+std::to_string(contrast)
          + ":saturation="+std::to_string(saturation);
    const char *filter_descr = temp.c_str();

    avfilter_graph_parse_ptr(filter_graph, filter_descr, &inputs, &outputs, nullptr);
    avfilter_graph_config(filter_graph, nullptr);
}

Filters::~Filters()
{
    avfilter_graph_free(&filter_graph);
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
}
