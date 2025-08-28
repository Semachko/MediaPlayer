#include "media/codec.h"
#include <QDebug>

Codec::Codec(AVStream* stream) {
    this->stream = stream;
    this->parameters = stream->codecpar;
    this->codec = avcodec_find_decoder(parameters->codec_id);
    this->context = avcodec_alloc_context3(codec);
    this->context->thread_count = 0;
    this->context->thread_type = FF_THREAD_FRAME | FF_THREAD_SLICE;
    avcodec_parameters_to_context(context, parameters);
    avcodec_open2(context, codec, nullptr);
}

Codec::~Codec(){
    avcodec_free_context(&context);
}
