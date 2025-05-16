#include "sampleconverter.h"
#include <QDebug>

SampleConverter::SampleConverter(AVCodecContext* input, SampleFormat output) : output_format(output)
{
    converter_context = swr_alloc();

    int ret = swr_alloc_set_opts2(
        &converter_context,
        &output_format.layout, (AVSampleFormat)output_format.format, output_format.sample_rate,
        &input->ch_layout, input->sample_fmt, input->sample_rate,
        0, nullptr
        );
    if (ret < 0) {
        qDebug() << "Wrong input/output options failed:" << ret;
        return;
    }
    ret = swr_init(converter_context);
    if (ret < 0) {
        qDebug() << "Creating convertor failed:" << ret;
        return;
    }
}

SampleConverter::~SampleConverter()
{
    swr_free(&converter_context);
}


Frame SampleConverter::convert(Frame input)
{
    Frame output = make_shared_frame();
    output->format = output_format.format;
    output->sample_rate = output_format.sample_rate;
    output->ch_layout = output_format.layout;
    output->nb_samples = av_rescale_rnd(
        swr_get_delay(converter_context, input->sample_rate) + input->nb_samples,
        output->sample_rate,
        input->sample_rate,
        AV_ROUND_UP
    );
    if (av_frame_get_buffer(output.get(), 0) < 0) {
        qDebug() << "Allocating output frame error";
        return output;
    }
    output->nb_samples = swr_convert(
        converter_context,
        output->data,
        output->nb_samples,
        (const uint8_t**)input->data,
        input->nb_samples
    );
    return output;
}

Frame SampleConverter::flush()
{
    Frame output = make_shared_frame();
    output->format = output_format.format;
    output->sample_rate = output_format.sample_rate;
    output->ch_layout = output_format.layout;

    int delay = swr_get_delay(converter_context, output->sample_rate);
    if (delay < 0 || output->sample_rate == 0){
        qDebug()<<"Wrong delay";
        return nullptr;
    }

    output->nb_samples = av_rescale_rnd(
        delay,
        output->sample_rate,
        output->sample_rate,
        AV_ROUND_UP
    );

    if (output->nb_samples <= 0){
        qDebug()<<"Wrong nb_samples";
        return nullptr;
    }

    if (av_frame_get_buffer(output.get(), 0) < 0){
        qDebug()<<"Wrong initializing frame buffer";
        return nullptr;
    }

    int ret = swr_convert(
        converter_context,
        output->data,
        output->nb_samples,
        nullptr, 0
    );

    if (ret <= 0){
        qDebug()<<"Error converting:" << ret;
        return nullptr;
    }
    output->nb_samples = ret;
    return output;
}


