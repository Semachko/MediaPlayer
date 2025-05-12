#include "equalizer.h"
#include <QDebug>

Equalizer::Equalizer(AVCodecContext* cod_ctx) : codec_context(cod_ctx){
    args =
        "sample_rate="+std::to_string(codec_context->sample_rate)
        +":sample_fmt="+av_get_sample_fmt_name(codec_context->sample_fmt)
        //+":sample_fmt=dblp"
        +":channel_layout=stereo"
        +":channels="+std::to_string(codec_context->ch_layout.nb_channels);

    //av_log_set_level(AV_LOG_DEBUG);

    update_equalizer();
}

// Equalizer::Equalizer(QAudioFormat& format, AVSampleFormat sampleFormat) {
//     args =
//         "sample_rate="+std::to_string(format.sampleRate())
//         +":sample_fmt="+av_get_sample_fmt_name(sampleFormat)
//         //+":sample_fmt=dblp"
//         //+":channel_layout=3"
//         +":channels="+std::to_string(format.channelCount());

//     av_log_set_level(AV_LOG_DEBUG);

//     update_equalizer();
// }

Frame Equalizer::applyEqualizer(AVFrame *frame)
{
    QMutexLocker _(&mutex);
    qDebug()<<"Input frame format:"<<frame->format;

    Frame equalized_frame;
    av_buffersrc_add_frame(buffersrc_ctx, frame);
    // while (av_buffersink_get_frame(buffersink_ctx, filtered_frame.get()) >= 0) {
    // }
    av_buffersink_get_frame(buffersink_ctx, equalized_frame.get());

    qDebug()<<"Equalized format:"<<equalized_frame->format;

    return equalized_frame;
}

void Equalizer::set_low(qreal value)
{
    qDebug()<<"low setted";
    low = value;
    update_equalizer();
}

void Equalizer::set_mid(qreal value)
{
    qDebug()<<"mid setted";
    mid = value;
    update_equalizer();
}

void Equalizer::set_high(qreal value)
{
    qDebug()<<"high setted";
    high = value;
    update_equalizer();
}

void Equalizer::update_equalizer()
{
    QMutexLocker _(&mutex);

    if (filter_graph) {
        avfilter_graph_free(&filter_graph);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
    }

    filter_graph = avfilter_graph_alloc();

    buffersrc  = avfilter_get_by_name("abuffer");
    buffersink = avfilter_get_by_name("abuffersink");

    qDebug()<<"Args: "<<args;
    if (avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args.c_str(), nullptr, filter_graph) < 0)
        qDebug() << "Failed to create abuffer";
    if (avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", nullptr, nullptr, filter_graph) < 0)
        qDebug() << "Failed to create abuffersink";

    // const AVSampleFormat out_sample_fmts[] = { codec_context->sample_fmt, AV_SAMPLE_FMT_NONE };
    // av_opt_set_int_list(buffersink_ctx, "sample_fmts", out_sample_fmts, AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN);

    // const int64_t out_channel_layouts[] = { codec_context->ch_layout.nb_channels, -1 };
    // av_opt_set_int_list(buffersink_ctx, "channel_layouts", out_channel_layouts, -1, AV_OPT_SEARCH_CHILDREN);

    // const int out_sample_rates[] = { codec_context->sample_rate, -1 };
    // av_opt_set_int_list(buffersink_ctx, "sample_rates", out_sample_rates, -1, AV_OPT_SEARCH_CHILDREN);


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

    //std::string filter_descr = "bass=g=20,treble=g=30";
    std::string filter_descr =
        "aformat=sample_fmts=fltp,equalizer=f=100:width_type=h:width=400:g=" + std::to_string(low)
        + ",equalizer=f=1000:width_type=h:width=2000:g=" + std::to_string(mid)
        + ",equalizer=f=8000:width_type=h:width=8000:g=" + std::to_string(high)
        + ",aformat=sample_fmts=flt";
    //,aformat=sample_fmts=fltp
    //std::string filter_descr = "volume=0.5";
    qDebug() << "filter_descr:" << filter_descr.c_str();

    if (avfilter_graph_parse_ptr(filter_graph, filter_descr.c_str(), &inputs, &outputs, nullptr) < 0)
        qDebug() << "Error to resolve filter";
    if (avfilter_graph_config(filter_graph, nullptr) < 0)
        qDebug() << "Error to configure graph";
}
