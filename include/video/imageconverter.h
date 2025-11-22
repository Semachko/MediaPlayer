#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>

#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
}
#include "frame.h"
#include "media/codec.h"

class ImageConverter {
    public:
        ImageConverter(const Codec&);
        ~ImageConverter();

        Frame convert(Frame input);

    private:
        Frame output;
        SwsContext* converter;
};

#endif  // IMAGECONVERTER_H
