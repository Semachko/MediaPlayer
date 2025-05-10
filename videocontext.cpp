#include <QImage>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QtConcurrent>

#include "videocontext.h"
#include "frame.h"

VideoContext::VideoContext(AVFormatContext *format_context, Synchronizer* sync, QVideoSink* videosink) : sync(sync), videosink(videosink), packetQueue(20)
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

    filters = new Filters(codec_parameters,time_base);

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

    output = new FrameOutput(sync,videosink);
    outputThread = new QThread(this);
    output->moveToThread(outputThread);
    outputThread->start();

    connect(output,&FrameOutput::imageOutputted, this, &VideoContext::push_frame_to_queue);
    QMetaObject::invokeMethod(output,&FrameOutput::start_output, Qt::QueuedConnection);

    connect(this,&VideoContext::newPacketArrived, this, &VideoContext::push_frame_to_queue);
}

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto IMAGE = "\033[35m[Image]\033[0m";


void VideoContext::push_frame_to_queue()
{
    sync->check_pause();
    QMutexLocker _(&decodingMutex);
    // qDebug()<<DECODING<<"New video packet arrived";
    // qDebug()<<DECODING<<"Packet queue size ="<<packetQueue.size();
    if (output->imageQueue.size()>=output->imageQueue.max_size)
        return;
    if (packetQueue.size()==0){
        //qDebug()<<DECODING<<"packet queue size = 0, requesting packet...";
        emit requestPacket();
        return;
    }

    {
        Packet packet = packetQueue.pop();
        emit requestPacket();
        //qDebug()<<DECODING<<"packet received, decoding... packet size ="<<packet.get()->size;
        int ret = avcodec_send_packet(codec_context, packet.get());

        if (ret < 0) {
            qDebug()<<"Error sending video packet: "<<ret;
            return;
        }
    }

    //AVFrame *frame = av_frame_alloc();
    Frame frame;
    while (avcodec_receive_frame(codec_context, frame.get()) == 0)
    {

        Frame filtered_frame = filters->applyFilters(frame.get());

        //qDebug()<<IMAGE<<"Received frame, formating it";
        sws_scale(
            frame_format,
            filtered_frame->data,
            filtered_frame->linesize,
            0,
            codec_context->height,
            rgbFrame->data,
            rgbFrame->linesize
            );
        QImage image(rgbFrame->data[0], codec_context->width, codec_context->height, rgbFrame->linesize[0], QImage::Format_RGB32);
        qint64 imagetime = filtered_frame->best_effort_timestamp * 1000 * time_base.num / time_base.den;
        ImageFrame imageFrame(std::move(QVideoFrame(image.copy())),imagetime);

        output->imageQueue.push(std::move(imageFrame));
        //qDebug()<<IMAGE<<"Notifying about new image";
        output->imageReady.notify_all();

        av_frame_unref(frame.get());
    }
    //av_frame_free(&frame);
}

void VideoContext::set_brightness(qreal value)
{
    qDebug("Invoking set_brightness");
    filters->set_brightness(value);
}

void VideoContext::set_contrast(qreal value)
{
    qDebug("Invoking set_contrast");
    filters->set_contrast(value);
}

void VideoContext::set_saturation(qreal value)
{
    qDebug("Invoking set_saturation");
    filters->set_saturation(value);
}


