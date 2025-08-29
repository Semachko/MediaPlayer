#include "video/frameoutput.h"
#include <QDebug>
#include <QThread>
#include <chrono>

constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

// ImageFrame::ImageFrame(QVideoFrame&& videoframe, qint64 time)
//     :
//     image(videoframe),
//     time(time)
// {}

FrameOutput::FrameOutput(QVideoSink * videosink, Synchronizer * sync, Codec& codec_, qint64 queueSize)
    :
    sync(sync),
    codec(codec_),
    videosink(videosink),
    image_queue(queueSize)
{}
FrameOutput::~FrameOutput()
{
    delete filters;
    delete converter;
}

void FrameOutput::start_output()
{
    while(!abort)
    {
        sync->check_pause();
        process_image();
    }
}

void FrameOutput::process_image()
{
        Frame frame = image_queue.wait_pop();
        copy_frame(frame, current_frame);
        Frame filtered_frame = filters->applyFilters(frame);
        Frame output_frame = converter->convert(filtered_frame);
        QImage image(output_frame->data[0], codec.context->width, codec.context->height, output_frame->linesize[0], QImage::Format_RGB32);
        QVideoFrame videoframe = QVideoFrame(image);
        qint64 frametime = filtered_frame->best_effort_timestamp * 1000 * timebase;
        qint64 delay = frametime - sync->get_time();
        //qDebug()<<"Delay: "<<delay;
        if (delay>0)
            QThread::msleep(delay);
        if (abort)
            return;
        videosink->setVideoFrame(videoframe);
        emit imageOutputted();
}

void FrameOutput::process_one_image()
{
{
        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        auto us = duration_cast<microseconds>(now.time_since_epoch()) % 1000;
        auto s  = duration_cast<seconds>(now.time_since_epoch());

        qDebug().noquote() << "start waiting " <<QString("%1:%2:%3")
        .arg((s.count()) % 60, 2, 10, QChar('0'))   // секунды
        .arg(ms.count(), 3, 10, QChar('0'))         // миллисекунды
        .arg(us.count(), 3, 10, QChar('0'));        // микросекунды
}

        Frame frame = image_queue.wait_pop();

{
        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        auto us = duration_cast<microseconds>(now.time_since_epoch()) % 1000;
        auto s  = duration_cast<seconds>(now.time_since_epoch());

        qDebug().noquote() << "got final image" <<QString("%1:%2:%3")
        .arg((s.count()) % 60, 2, 10, QChar('0'))   // секунды
        .arg(ms.count(), 3, 10, QChar('0'))         // миллисекунды
        .arg(us.count(), 3, 10, QChar('0'));        // микросекунды
}


        copy_frame(frame, current_frame);
        Frame filtered_frame = filters->applyFilters(frame);
        Frame output_frame = converter->convert(filtered_frame);
        QImage image(output_frame->data[0], codec.context->width, codec.context->height, output_frame->linesize[0], QImage::Format_RGB32);
        QVideoFrame videoframe = QVideoFrame(image);
        videosink->setVideoFrame(videoframe);
        emit imageOutputted();
}

void FrameOutput::set_filters_on_currentFrame()
{
        Frame frame = make_shared_frame();
        copy_frame(current_frame, frame);
        Frame filtered_frame = filters->applyFilters(frame);
        Frame output_frame = converter->convert(filtered_frame);
        QImage image(output_frame->data[0], codec.context->width, codec.context->height, output_frame->linesize[0], QImage::Format_RGB32);
        QVideoFrame videoframe = QVideoFrame(image);
        videosink->setVideoFrame(videoframe);
        emit imageOutputted();
}
void FrameOutput::copy_frame(Frame source, Frame destination)
{
        av_frame_unref(destination.get());
        destination->format = source->format;
        destination->width  = source->width;
        destination->height = source->height;
        av_frame_get_buffer(destination.get(), 32);
        av_frame_copy(destination.get(),source.get());
        av_frame_copy_props(destination.get(),source.get());
}

