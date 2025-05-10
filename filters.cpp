#include "filters.h"
#include <QDebug>

Filters::Filters(AVCodecParameters* codec_parameters, AVRational time_base)
{
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=1/1",
             codec_parameters->width, codec_parameters->height, codec_parameters->format, time_base.num, time_base.den);

    update_filters();

    // filter_graph = avfilter_graph_alloc();

    // buffersrc  = avfilter_get_by_name("buffer");
    // buffersink = avfilter_get_by_name("buffersink");
    // avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args, NULL, filter_graph);
    // avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", NULL, NULL, filter_graph);

    // outputs = avfilter_inout_alloc();
    // outputs->name       = av_strdup("in");
    // outputs->filter_ctx = buffersrc_ctx;
    // outputs->pad_idx    = 0;
    // outputs->next       = NULL;

    // inputs  = avfilter_inout_alloc();
    // inputs->name       = av_strdup("out");
    // inputs->filter_ctx = buffersink_ctx;
    // inputs->pad_idx    = 0;
    // inputs->next       = NULL;

    // std::string temp
    //     = "eq=brightness="+std::to_string(brightness)
    //     + ":contrast="+std::to_string(contrast)
    //     + ":saturation="+std::to_string(saturation);
    // const char *filter_descr = temp.c_str();

    // avfilter_graph_parse_ptr(filter_graph, filter_descr, &inputs, &outputs, NULL);
    // avfilter_graph_config(filter_graph, NULL);
}

Frame Filters::applyFilters(AVFrame* frame)
{
    QMutexLocker _(&mutex);
    Frame filtered_frame;
    av_buffersrc_add_frame(buffersrc_ctx, frame);
    // while (av_buffersink_get_frame(buffersink_ctx, filtered_frame.get()) >= 0) {
    // }
    av_buffersink_get_frame(buffersink_ctx, filtered_frame.get());
    return filtered_frame;
}

void Filters::set_brightness(qreal value)
{
    qDebug()<<"Brightness setted";
    brightness = value;
    update_filters();
}

void Filters::set_contrast(qreal value)
{
    qDebug()<<"Contrast setted";
    contrast = value;
    update_filters();
}

void Filters::set_saturation(qreal value)
{
    qDebug()<<"Saturation setted";
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
    avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args, nullptr, filter_graph);
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
