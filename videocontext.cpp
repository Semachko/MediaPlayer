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

void VideoContext::push_frame_to_queue()
{
    //qDebug()<<"\033[31m[Decoding]\033[0m New video packet arrived";
    AVPacket* packet = packetQueue.front();
    packetQueue.pop();

    //qDebug()<<"\033[31m[Decoding]\033[0m Decoding packet";
    int ret = avcodec_send_packet(codec_context, packet);
    av_packet_free(&packet);
    if (ret < 0) {
        qDebug()<<"Error sending video packet: "<<ret;
        return;
    }

    AVFrame *frame = av_frame_alloc();
    while (avcodec_receive_frame(codec_context, frame) == 0)
    {
        //qDebug()<<"\033[35m[Image]\033[0m Received frame, formating it";
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
        //qDebug()<<"\033[35m[Image]\033[0m Notifying about new image";
        output->imageReady.notify_all();

        av_frame_unref(frame);
    }
    av_frame_free(&frame);
}


void VideoContext::start_output()
{
    // while(true)
    // {
    //     sync->check_pause();

    //     QMutexLocker locker(&imageMutex);
    //     qDebug()<<"\033[34m[Output]\033[0m Checking is image queue empty";
    //     while(imageQueue.empty()){
    //         qDebug()<<"\033[34m[Output]\033[0m Queue is empty, waiting for images";
    //         imageReady.wait(&imageMutex);
    //     }
    //     qDebug()<<"\033[34m[Output]\033[0m Queue contain image, outputing";

    //     qDebug()<<"IMAGE OBJ SIZE = "<<sizeof(imageQueue.front());

    //     qint64 delay = imageQueue.front().second - sync->get_time();
    //     qDebug()<<"Video time = "<<imageQueue.front().second;
    //     qDebug()<<"Clock time = "<<sync->get_time();
    //     qDebug()<<"Delay: "<<delay;
    //     if (delay>0)
    //         QThread::msleep(delay);
    //     output_image();
    // }
}

void VideoContext::output_image()
{
    // qDebug()<<"\033[34m[Output]\033[0m Signaling to output image";
    // QVideoFrame image = imageQueue.front().first;
    // imageQueue.pop();
    // emit imageToOutput(image);
}



void VideoContext::synchronize()
{
    // QMutexLocker locker(&sync->pauseMutex);
    // while(sync->isPaused)
    //     sync->pauseWait.wait(&sync->pauseMutex);
}

void VideoContext::process(AVPacket* packet)
{
    // int ret = avcodec_send_packet(codec_context, packet);
    // av_packet_free(&packet);
    // if (ret < 0) {
    //     qDebug()<<"Error sending video packet: "<<ret;
    //     return;
    // }

    // AVFrame *frame = av_frame_alloc();
    // while (avcodec_receive_frame(codec_context, frame) == 0)
    // {
    //     sws_scale(
    //         frame_format,
    //         frame->data,
    //         frame->linesize,
    //         0,
    //         codec_context->height,
    //         rgbFrame->data,
    //         rgbFrame->linesize
    //     );
    //     QImage image = QImage(rgbFrame->data[0], codec_context->width, codec_context->height, rgbFrame->linesize[0], QImage::Format_RGB32);
    //     QVideoFrame qframe = QVideoFrame(image.copy());
    //     //qDebug()<<"qVideoFrame size = " << qframe.size();

    //     synchronize();

    //     imagetime = frame->best_effort_timestamp * 1000 * time_base.num / time_base.den;
    //     if (!qframe.size().isEmpty()){
    //         qint64 delay = imagetime - sync->clock->get_time()-10;
    //         //qDebug()<<"Video time = "<<imagetime;
    //         //qDebug()<<"Clock time = "<<sync->clock->getClock();
    //         qDebug()<<"Delay: "<<delay;
    //         if (delay>0)
    //             QThread::msleep(delay);
    //         emit imageToOutput(qframe);
    //     }
    //     av_frame_unref(frame);
    // }
    // av_frame_free(&frame);
    // return;
}

