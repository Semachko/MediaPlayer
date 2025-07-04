﻿#include <QImage>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QtConcurrent>

#include "video/videocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto IMAGE = "\033[35m[Image]\033[0m";

VideoContext::VideoContext(QVideoSink* videosink, AVFormatContext* format_context, Synchronizer* sync, int stream_id, qreal bufferization_time)
    :
    IMediaContext(10),
    videosink(videosink),
    stream_id(stream_id),
    sync(sync)
{
    timeBase = format_context->streams[stream_id]->time_base;

    // Initiating CODEC
    AVCodecParameters* codec_parameters = format_context->streams[stream_id]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_parameters->codec_id);
    codec_context = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_context, codec_parameters);
    avcodec_open2(codec_context, codec, nullptr);



    qreal fps = av_q2d(format_context->streams[stream_id]->avg_frame_rate);
    maxBufferSize = fps * bufferization_time;


    output = new FrameOutput(videosink, sync, maxBufferSize);
    output->filters = new Filters(codec_parameters,timeBase);
    output->converter = new ImageConverter(codec_context);
    output->codec_context = codec_context;
    output->timebase = av_q2d(timeBase);
    outputThread = new QThread(this);
    output->moveToThread(outputThread);
    outputThread->start();

    connect(output,&FrameOutput::imageOutputted, this, &VideoContext::decode_and_output);
    QMetaObject::invokeMethod(output,&FrameOutput::start_output, Qt::QueuedConnection);

    connect(this,&VideoContext::newPacketArrived, this, &VideoContext::decode_and_output);
}

VideoContext::~VideoContext()
{
    if (stream_id<0)
        return;
    avcodec_free_context(&codec_context);
    // delete filters;
    // delete converter;

    output->imageReady.wakeAll();
    outputThread->quit();
    outputThread->wait();
    output->deleteLater();
    outputThread->deleteLater();

    delete imageReady;
}


void VideoContext::decode_and_output()
{
    QMutexLocker _(&videoMutex);

    if (buffer_available() == 0)
        return;
    Packet packet;
    if(!packetQueue.pop(packet)){
        emit requestPacket();
        return;
    }
    decode(packet);
    filter_and_output();
}
void VideoContext::decode(Packet& packet)
{
    qreal packetTime = packet->pts * av_q2d(timeBase);
    qreal currTime = sync->get_time() / 1000.0;
    qreal diff = currTime - packetTime;
    if (diff > 0.15){
        qDebug()<<"diff ="<<diff<<"Lating, skip.";
        emit requestPacket();
        return;
    }
    int ret = avcodec_send_packet(codec_context, packet.get());
    if (ret < 0) {
        qDebug()<<"Error decoding video packet: "<<ret;
        return;
    }
}

void VideoContext::filter_and_output()
{
    Frame frame = make_shared_frame();
    int res;
    while ((res = avcodec_receive_frame(codec_context, frame.get())) == 0)
    {
        // Frame filtered_frame = filters->applyFilters(frame);
        // Frame output_frame = converter->convert(filtered_frame);

        //QImage image(output_frame->data[0], codec_context->width, codec_context->height, output_frame->linesize[0], QImage::Format_RGB32);
        //qint64 imagetime = filtered_frame->best_effort_timestamp * 1000 * timeBase.num / timeBase.den;
        //ImageFrame imageFrame(std::move(QVideoFrame(image.copy())),imagetime);
        Frame output_frame = make_shared_frame();
        av_frame_move_ref(output_frame.get(), frame.get());
        output->imageQueue.push(output_frame);
        output->imageReady.notify_all();

        //av_frame_unref(frame.get());
    }
}

qint64 VideoContext::buffer_available()
{
    qint64 available_size = maxBufferSize - output->imageQueue.size();
    if (available_size <= 0)
        return 0;
    return available_size;
}

void VideoContext::set_brightness(qreal value)
{
    output->filters->set_brightness(value);
    if (sync->isPaused)
        output->set_filters_on_currentFrame();
}

void VideoContext::set_contrast(qreal value)
{
    output->filters->set_contrast(value);
    if (sync->isPaused)
        output->set_filters_on_currentFrame();
}

void VideoContext::set_saturation(qreal value)
{
    output->filters->set_saturation(value);
    if (sync->isPaused)
        output->set_filters_on_currentFrame();
}
