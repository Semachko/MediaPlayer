#include <QImage>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QtConcurrent>

#include "videocontext.h"

VideoContext::VideoContext(AVFormatContext *format_context, Synchronizer* sync) : sync(sync)
{
    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (stream_id<0)
        return;

    time_base = format_context->streams[stream_id]->time_base;

    AVCodecParameters* codec_parameters = format_context->streams[stream_id]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_parameters->codec_id);

    codec_context = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_context, codec_parameters);
    avcodec_open2(codec_context, codec, nullptr);

    frame_format = sws_getContext(
        codec_context->width,
        codec_context->height,
        codec_context->pix_fmt,
        codec_context->width,
        codec_context->height,
        AV_PIX_FMT_RGB32,
        SWS_BICUBIC, nullptr, nullptr, nullptr
    );

    buffer = std::vector<uint8_t>(
        av_image_get_buffer_size(
            AV_PIX_FMT_RGB32,
            codec_context->width,
            codec_context->height,
            1
        )
    );

    rgbFrame = av_frame_alloc();
    av_image_fill_arrays(
        rgbFrame->data,
        rgbFrame->linesize,
        buffer.data(),
        AV_PIX_FMT_RGB32,
        codec_context->width,
        codec_context->height,
        1
    );

    output = new FrameOutput(sync);
    outputThread = new QThread(this);
    output->moveToThread(outputThread);
    outputThread->start();
    QMetaObject::invokeMethod(output, [this](){
        output->start_output();
    });

    connect(this,&VideoContext::newPacketReady,this,&VideoContext::push_frame_to_queue);
}

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto IMAGE = "\033[35m[Image]\033[0m";

void VideoContext::push_frame_to_queue()
{
    //qDebug()<<DECODING<<"New video packet arrived";
    queueMutex.lock();
    AVPacket* packet = packetQueue.front();
    packetQueue.pop();
    queueMutex.unlock();

    //qDebug()<<DECODING<<"Decoding packet";
    int ret = avcodec_send_packet(codec_context, packet);
    av_packet_free(&packet);
    if (ret < 0) {
        //qDebug()<<"Error sending video packet: "<<ret;
        return;
    }

    AVFrame *frame = av_frame_alloc();
    while (avcodec_receive_frame(codec_context, frame) == 0)
    {
        //qDebug()<<IMAGE<<"Received frame, formating it";
        sws_scale(
            frame_format,
            frame->data,
            frame->linesize,
            0,
            codec_context->height,
            rgbFrame->data,
            rgbFrame->linesize
            );
        QImage image(rgbFrame->data[0], codec_context->width, codec_context->height, rgbFrame->linesize[0], QImage::Format_RGB32);
        qint64 imagetime = frame->best_effort_timestamp * 1000 * time_base.num / time_base.den;

        ImageFrame imageFrame(std::move(QVideoFrame(image.copy())),imagetime);

        output->push_imageQueue(std::move(imageFrame));
        //qDebug()<<IMAGE<<"Notifying about new image";
        output->imageReady.notify_all();

        av_frame_unref(frame);
    }
    av_frame_free(&frame);
}


