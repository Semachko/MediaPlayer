#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

extern "C" {
#include "libavformat/avformat.h"
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/avutil.h>
}

class ImageConverter
{
public:
    ImageConverter();
    AVFrame* convert(AVFrame* input);

private:
    SwsContext* converter_context;

};

#endif // IMAGECONVERTER_H
