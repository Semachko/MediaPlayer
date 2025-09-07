#include "media/codec.h"
#include <QDebug>

Codec::Codec(AVStream* stream, int thread_count) {
    this->stream = stream;
    this->parameters = stream->codecpar;
    this->codec = avcodec_find_decoder(parameters->codec_id);
    if (!codec)
        throw std::runtime_error("Decoder not found");
    this->context = avcodec_alloc_context3(codec);
    if (!context)
        throw std::runtime_error("Failed to allocate codec context");
    if (avcodec_parameters_to_context(context, parameters) < 0)
        throw std::runtime_error("Failed to copy codec parameters to context");
    this->context->thread_count = 0;
    this->context->thread_type = FF_THREAD_FRAME | FF_THREAD_SLICE;
    if (thread_count == 1)
        this->context->thread_type = 0;
    if (avcodec_open2(context, codec, nullptr) < 0)
        throw std::runtime_error("Failed to open codec");
    this->timeBase = stream->time_base;
}

Codec::~Codec(){
    avcodec_free_context(&context);
}
