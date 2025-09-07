#include "video/videopreview.h"
#include <QImage>
#include <QVideoFrame>

VideoPreview::VideoPreview(MediaParameters *par)
    : videosink(par->video->previewSink),
    params(par)
{
    avformat_open_input(&format_context, params->file->path.toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);
    int stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    codec = new Codec(format_context->streams[stream_id], 1);
    decoder = new Decoder(*codec);
    converter = new ImageConverter(*codec);
    connect(params->video,&VideoParameters::setPreview,this, [this](qint64 timepoint){update_preview(timepoint);});
    params->video->is_preview_processing = false;
}

VideoPreview::~VideoPreview()
{
    videosink->setVideoFrame(QVideoFrame());
    delete converter;
    delete decoder;
    delete codec;
    avformat_close_input(&format_context);
}

void VideoPreview::update_preview(qint64 timepoint)
{
    //qDebug()<<"Current is_preview_processing:"<<params->video->is_preview_processing;
    decoder->clear_decoder();
    int64_t ts = timepoint / 1000.0 / av_q2d(codec->timeBase);
    av_seek_frame(format_context, codec->stream->index, ts, AVSEEK_FLAG_BACKWARD);
    for(;;)
    {
        Packet packet = make_shared_packet();
        int res = av_read_frame(format_context, packet.get());
        if (res == AVERROR_EOF)
            return;
        if (packet->stream_index != codec->stream->index)
            continue;
        decoder->decode_packet(packet);
        auto queue = decoder->receive_frames();
        while(!queue.empty())
        {
            Frame frame = queue.dequeue();
            qint64 frametime = frame->best_effort_timestamp * 1000 * av_q2d(codec->timeBase);
            if (frametime < timepoint)
                continue;
            Frame output_frame = converter->convert(frame);
            QImage image(output_frame->data[0], codec->context->width, codec->context->height, output_frame->linesize[0], QImage::Format_RGB32);
            videosink->setVideoFrame(QVideoFrame(image));
            params->video->is_preview_processing = false;
            return;
        }
    }
}









