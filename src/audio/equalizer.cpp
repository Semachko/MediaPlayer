#include "audio/equalizer.h"
#include <QDebug>

Equalizer::Equalizer(Codec& codec_, MediaParameters* params_)
    : params(params_),
    codec(codec_)
{
    //av_log_set_level(AV_LOG_DEBUG);
    char buf[128];
    av_channel_layout_describe(&codec.context->ch_layout, buf, sizeof(buf));
    args =
        "sample_rate="+std::to_string(codec.context->sample_rate)
        +":sample_fmt="+av_get_sample_fmt_name(codec.context->sample_fmt)
        +":channel_layout=" + buf
        +":channels="+std::to_string(codec.context->ch_layout.nb_channels);
    update_equalizer();
    connect(params,&MediaParameters::speedChanged,this,&Equalizer::update_equalizer);
    connect(params->audio,&AudioParameters::paramsChanged,this,&Equalizer::update_equalizer);
}

Frame Equalizer::applyEqualizer(Frame frame)
{
    std::lock_guard _(mutex);
    // int best_effort_timestamp = frame->best_effort_timestamp;
    // int pts = frame->pts;
    int ret = av_buffersrc_add_frame(buffersrc_ctx, frame.get());
    if (ret < 0) {
        qDebug() << "av_buffersrc_add_frame error:" << ret;
        return nullptr;
    }
    Frame equalized_frame = make_shared_frame();
    ret = av_buffersink_get_frame(buffersink_ctx, equalized_frame.get());
    if (ret < 0) {
        qDebug() << "av_buffersink_get_frame error:" << ret;
        return nullptr;
    }
    // qreal timestamp_diff = (equalized_frame->best_effort_timestamp - best_effort_timestamp) * av_q2d(codec.timeBase);
    // qreal pts_diff = (equalized_frame->pts - pts) * av_q2d(codec.timeBase);
    // qDebug()<<"timestamp_diff:"<<timestamp_diff;
    // qDebug()<<"pts_diff:"<<pts_diff;
    return equalized_frame;
}

void Equalizer::update_equalizer()
{
    std::lock_guard _(mutex);

    if (filter_graph) {
        avfilter_graph_free(&filter_graph);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
    }

    filter_graph = avfilter_graph_alloc();

    buffersrc  = avfilter_get_by_name("abuffer");
    buffersink = avfilter_get_by_name("abuffersink");

    //qDebug()<<"Args: "<<args;
    if (avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args.c_str(), nullptr, filter_graph) < 0)
        qDebug() << "Failed to create abuffer";
    if (avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", nullptr, nullptr, filter_graph) < 0)
        qDebug() << "Failed to create abuffersink";


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

    std::string filter_descr =
        "atempo=" + std::to_string(params->speed)
        + std::string(",aformat=sample_fmts=fltp")
        + ",equalizer=f=100:width_type=h:width=400:g=" + std::to_string(params->audio->low)
        + ",equalizer=f=1000:width_type=h:width=2000:g=" + std::to_string(params->audio->mid)
        + ",equalizer=f=8000:width_type=h:width=8000:g=" + std::to_string(params->audio->high);

    //qDebug()<<"filter_descr:"<<filter_descr;
    if (avfilter_graph_parse_ptr(filter_graph, filter_descr.c_str(), &inputs, &outputs, nullptr) < 0)
        qDebug() << "Error to resolve filter";
    if (avfilter_graph_config(filter_graph, nullptr) < 0)
        qDebug() << "Error to configure graph";
}


Equalizer::~Equalizer()
{
    avfilter_graph_free(&filter_graph);
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
}
