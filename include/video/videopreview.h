#ifndef VIDEOPREVIEW_H
#define VIDEOPREVIEW_H

extern "C" {
#include "libavformat/avformat.h"
#include <libavcodec/avcodec.h>
}

#include "media/codec.h"
#include "media/decoder.h"
#include "params/mediaparameters.h"
#include "video/imageconverter.h"

#include <QObject>
#include <QVideoSink>

class VideoPreview : public QObject {
        Q_OBJECT
    public:
        VideoPreview(MediaParameters* par);
        ~VideoPreview();
        void update_preview(qreal seconds);

    private:
        int stream_id = -1;
        AVFormatContext* format_context = nullptr;
        Codec* codec;
        Decoder* decoder;
        ImageConverter* converter;
        MediaParameters* params;
        QVideoSink* videosink;
};

#endif  // VIDEOPREVIEW_H
