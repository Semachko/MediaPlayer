#ifndef SAMPLECONVERTER_H
#define SAMPLECONVERTER_H

extern "C" {
#include "libavformat/avformat.h"
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/avutil.h>
}

class SampleConverter
{
public:
    SampleConverter();
    AVFrame* convert(AVFrame* input);

    void set_out_format(AVSampleFormat);
    void set_out_sample_rate(int);
    void set_out_layout(AVChannelLayout);
private:
    SwrContext* converter_context;

    int out_format;
    int out_sample_rate;
    AVChannelLayout out_layout;
};

#endif // SAMPLECONVERTER_H
