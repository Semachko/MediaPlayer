#include <QImage>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QtConcurrent>

#include "videocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto IMAGE = "\033[35m[Image]\033[0m";

VideoContext::VideoContext(AVFormatContext *format_context, Synchronizer* sync, QVideoSink* videosink) : IMediaContext(20), sync(sync), videosink(videosink)
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
    converter = new ImageConverter(codec_context);

    output = new FrameOutput(sync,videosink);
    outputThread = new QThread(this);
    output->moveToThread(outputThread);
    outputThread->start();

    connect(output,&FrameOutput::imageOutputted, this, &VideoContext::push_frame_to_buffer);
    QMetaObject::invokeMethod(output,&FrameOutput::start_output, Qt::QueuedConnection);

    connect(this,&VideoContext::newPacketArrived, this, &VideoContext::push_frame_to_buffer);
}



void VideoContext::push_frame_to_buffer()
{
    sync->check_pause();
    QMutexLocker _(&decodingMutex);
    if (output->imageQueue.size()>=output->imageQueue.max_size)
        return;
    if (packetQueue.size()==0){
        emit requestPacket();
        return;
    }

    Packet packet = packetQueue.pop();
    emit requestPacket();

    int ret = avcodec_send_packet(codec_context, packet.get());
    if (ret < 0) {
        qDebug()<<"Error sending video packet: "<<ret;
        return;
    }

    Frame frame = make_shared_frame();
    while (avcodec_receive_frame(codec_context, frame.get()) == 0)
    {
        Frame filtered_frame = filters->applyFilters(frame);
        Frame output_frame = converter->convert(filtered_frame);

        QImage image(output_frame->data[0], codec_context->width, codec_context->height, output_frame->linesize[0], QImage::Format_RGB32);
        qint64 imagetime = filtered_frame->best_effort_timestamp * 1000 * time_base.num / time_base.den;
        ImageFrame imageFrame(std::move(QVideoFrame(image.copy())),imagetime);

        output->imageQueue.push(std::move(imageFrame));
        output->imageReady.notify_all();

        av_frame_unref(frame.get());
    }
}

void VideoContext::set_brightness(qreal value)
{
    filters->set_brightness(value);
}

void VideoContext::set_contrast(qreal value)
{
    filters->set_contrast(value);
}

void VideoContext::set_saturation(qreal value)
{
    filters->set_saturation(value);
}

void VideoContext::convert_and_push_images()
{

}


