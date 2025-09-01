#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

extern "C" {
#include "libavformat/avformat.h"
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/avutil.h>
#include "libavutil/imgutils.h"
}
#include "frame.h"
#include "media/codec.h"

class ImageConverter
{
public:
    ImageConverter(Codec&);
    ~ImageConverter();

    Frame convert(Frame input);
private:
    Frame output;
    SwsContext* converter;
};

#endif // IMAGECONVERTER_H
