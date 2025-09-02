#include "video/frameoutput.h"
#include <QDebug>
#include <QThread>

constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

// ImageFrame::ImageFrame(QVideoFrame&& videoframe, qint64 time)
//     :
//     image(videoframe),
//     time(time)
// {}

FrameOutput::FrameOutput(Synchronizer * sync, Codec& codec_, MediaParameters* params, qint64 queueSize)
    :
    sync(sync),
    codec(codec_),
    converter(codec),
    filters(codec, params->video),
    videosink(params->videoSink),
    image_queue(queueSize)
{}

void FrameOutput::start_output(){
    while(!abort)
    {
        sync->check_pause();
        process_image();
    }
}

void FrameOutput::process_image()
{
    Frame frame = image_queue.try_pop();
    if (!frame)
        return;
    copy_frame(frame, current_frame);
    Frame filtered_frame = filters.applyFilters(frame);
    Frame output_frame = converter.convert(filtered_frame);
    QImage image(output_frame->data[0], codec.context->width, codec.context->height, output_frame->linesize[0], QImage::Format_RGB32);
    QVideoFrame videoframe = QVideoFrame(image);
    qint64 frametime = filtered_frame->best_effort_timestamp * 1000 * av_q2d(codec.timeBase);
    qint64 delay = frametime - sync->get_time();
    //qDebug()<<"Delay: "<<delay;
    if (delay>0)
        QThread::msleep(delay);
    videosink->setVideoFrame(videoframe);
    emit imageOutputted();
}

void FrameOutput::process_one_image()
{
    Frame frame = image_queue.wait_pop();
    copy_frame(frame, current_frame);
    Frame filtered_frame = filters.applyFilters(frame);
    Frame output_frame = converter.convert(filtered_frame);
    QImage image(output_frame->data[0], codec.context->width, codec.context->height, output_frame->linesize[0], QImage::Format_RGB32);
    QVideoFrame videoframe{std::move(image)};
    videosink->setVideoFrame(videoframe);
    emit imageOutputted();
}

void FrameOutput::set_filters_on_currentFrame()
{
    Frame frame = make_shared_frame();
    copy_frame(current_frame, frame);
    Frame filtered_frame = filters.applyFilters(frame);
    Frame output_frame = converter.convert(filtered_frame);
    QImage image(output_frame->data[0], codec.context->width, codec.context->height, output_frame->linesize[0], QImage::Format_RGB32);
    QVideoFrame videoframe = QVideoFrame(image);
    videosink->setVideoFrame(videoframe);
    emit imageOutputted();
}

void FrameOutput::copy_frame(Frame source, Frame destination)
{
    std::lock_guard _(mutex);
    av_frame_unref(destination.get());
    destination->format = source->format;
    destination->width  = source->width;
    destination->height = source->height;
    av_frame_get_buffer(destination.get(), 32);
    av_frame_copy(destination.get(),source.get());
    av_frame_copy_props(destination.get(),source.get());
}

