#ifndef VIDEOPREVIEW_H
#define VIDEOPREVIEW_H

#include "video/imageconverter.h"
extern "C" {
#include "libavformat/avformat.h"
#include <libavcodec/avcodec.h>
}
#include <QObject>
#include <QVideoSink>
#include <media/mediaparameters.h>
#include <media/decoder.h>
#include <media/codec.h>

class VideoPreview : public QObject
{
    Q_OBJECT
public:
    VideoPreview(MediaParameters* par);
    ~VideoPreview();
    void update_preview(qint64 timepoint);
private:
    int stream_id = -1;
    AVFormatContext* format_context = nullptr;
    Codec* codec;
    Decoder* decoder;
    ImageConverter* converter;
    MediaParameters* params;
    QVideoSink* videosink;
};

#endif // VIDEOPREVIEW_H
