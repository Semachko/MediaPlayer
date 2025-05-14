#include "sampleconverter.h"
#include <QDebug>

SampleConverter::SampleConverter() {
    converter_context = swr_alloc();
    out_format = -1;
    out_sample_rate = -1;
    out_layout.nb_channels = -1;
}

AVFrame *SampleConverter::convert(AVFrame *input)
{
    swr_close(converter_context);
    AVFrame* output = av_frame_alloc();

    if (out_format == -1)
        out_format = input->sample_rate;
    if (out_sample_rate == -1)
        out_sample_rate = static_cast<AVSampleFormat>(input->format);
    if (out_layout.nb_channels == -1)
        out_layout = input->ch_layout;

    output->format = out_format;
    output->sample_rate = out_sample_rate;
    output->ch_layout = out_layout;

    int ret;
    ret = swr_alloc_set_opts2(
        &converter_context,
        &output->ch_layout, static_cast<AVSampleFormat>(output->format), output->sample_rate,
        &input->ch_layout, static_cast<AVSampleFormat>(input->format), input->sample_rate,
        0, nullptr
    );
    if (ret < 0) {
        qDebug() << "Wrong input/output options failed:" << ret;
        return output;
    }
    ret = swr_init(converter_context);
    if (ret < 0) {
        qDebug() << "Creating convertor failed:" << ret;
        return output;
    }
    output->nb_samples = av_rescale_rnd(
        swr_get_delay(converter_context, input->sample_rate) + input->nb_samples,
        output->sample_rate,
        input->sample_rate,
        AV_ROUND_UP
    );
    if ((ret = av_frame_get_buffer(output, 0)) < 0) {
        qDebug() << "Allocating output frame error" << ret;
        return output;
    }
    ret = swr_convert(
        converter_context,
        output->data,
        output->nb_samples,
        (const uint8_t**)input->data,
        input->nb_samples
    );
    if (ret<1)
        qDebug()<<"Converting ERROR";

    return output;
}

void SampleConverter::set_out_format(AVSampleFormat format)
{
    out_format = format;
}
void SampleConverter::set_out_sample_rate(int sample_rate)
{
    out_sample_rate = sample_rate;
}
void SampleConverter::set_out_layout(AVChannelLayout ch_layout)
{
    out_layout = ch_layout;
}


