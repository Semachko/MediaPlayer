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
#include <QAudioSink>
#include "frame.h"

struct SampleFormat{
    int format;
    int sample_rate;
    AVChannelLayout layout;
};


class SampleConverter
{
public:
    SampleConverter(AVCodecContext* input, SampleFormat output);
    Frame convert(Frame input);
    Frame flush();

private:
    SwrContext* converter_context;
    SampleFormat output_format;
};


#endif // SAMPLECONVERTER_H
