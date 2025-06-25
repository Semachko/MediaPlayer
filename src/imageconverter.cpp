#include "video/imageconverter.h"
#include <QDebug>

ImageConverter::ImageConverter(AVCodecContext* codec_context) {
    output = make_shared_frame();
    output->width  = codec_context->width;
    output->height = codec_context->height;
    output->format = AV_PIX_FMT_RGB32;

    if (av_frame_get_buffer(output.get(), 1) < 0){
        qDebug()<<"Error alocating output image convertor";
        return;
    }
    converter = sws_getContext(
        codec_context->width,
        codec_context->height,
        codec_context->pix_fmt,
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
