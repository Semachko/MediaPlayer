#ifndef CODEC_H
#define CODEC_H

extern "C" {
#include <libavcodec/avcodec.h>

#include "libavformat/avformat.h"
}

class Codec {
    public:
        Codec(AVStream* stream, int thread_count = 0);
        ~Codec();

    public:
        AVStream* stream;
        const AVCodec* codec;
        AVCodecContext* context;
        AVCodecParameters* parameters;
        AVRational timeBase;
};

#endif  // CODEC_H
