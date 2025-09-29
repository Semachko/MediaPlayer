#include "video/imageconverter.h"
#include <QDebug>

ImageConverter::ImageConverter(const Codec& codec) {
    output = make_shared_frame();
    output->width  = codec.context->width;
    output->height = codec.context->height;
    output->format = AV_PIX_FMT_RGB32;

    if (av_frame_get_buffer(output.get(), 1) < 0){
        qDebug()<<"Error alocating output image convertor";
        return;
    }
    converter = sws_getContext(
        codec.context->width,
        codec.context->height,
        codec.context->pix_fmt,
        output->width,
        output->height,
        static_cast<AVPixelFormat>(output->format),
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
}

ImageConverter::~ImageConverter()
{
    sws_freeContext(converter);
}

Frame ImageConverter::convert(Frame input)
{
    sws_scale(
        converter,
        input->data,
        input->linesize,
        0,
        output->height,
        output->data,
        output->linesize
    );
    return output;
}
