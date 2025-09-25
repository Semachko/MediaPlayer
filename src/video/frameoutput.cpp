#include "video/frameoutput.h"
#include <QDebug>
#include <QThread>
#include <QMediaMetaData>

#include <chrono>
std::string get_cur_time()
{
    using namespace std::chrono;
    auto now = system_clock::now().time_since_epoch();
    auto ms = duration_cast<milliseconds>(now).count() % 1000;
    auto us = duration_cast<microseconds>(now).count() % 1000;
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(3) << ms << ":"
        << std::setw(3) << us;
    return oss.str();
}

constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

// ImageFrame::ImageFrame(QVideoFrame&& videoframe, qint64 time)
//     :
//     image(videoframe),
//     time(time)
// {}

FrameOutput::FrameOutput(Clock* sync, Codec& codec_, MediaParameters* par, qint64 queueSize)
    :
    clock(sync),
    codec(codec_),
    converter(codec),
    filters(codec, par->video),
    videosink(par->videoSink),
    params(par),
    image_queue(queueSize)
{
    connect(this,&FrameOutput::outputImage,this, &FrameOutput::process_image, Qt::QueuedConnection);
    connect(params,&MediaParameters::isPausedChanged,this, [this]{if(!params->isPaused) emit outputImage();});
    if (!params->isPaused)
        emit outputImage();
}

FrameOutput::~FrameOutput()
{
    QMediaMetaData data;
    videosink->setVideoFrame(QVideoFrame());
}

void FrameOutput::process_image()
{
    std::lock_guard _(mutex);
    while(!image_queue.empty()){
        Frame frame = image_queue.try_pop();
        if (!frame)
            continue;
        emit requestImage();
        qreal currtime = clock->get_time();
        qreal frametime = frame->best_effort_timestamp * av_q2d(codec.timeBase);
        qreal delay = frametime - currtime;
        if (delay<0)
            continue;
        copy_frame(frame, current_frame);
        QVideoFrame videoframe = filter_and_convert(frame);
        delay = frametime - clock->get_time();
        if (delay>0)
            sleeper.wait(delay);
        videosink->setVideoFrame(videoframe);
        if(params->isPaused)
            return;
    }
    if(!params->isPaused)
        emit outputImage();
}

void FrameOutput::process_one_image()
{
    for(;;){
        Frame frame = image_queue.wait_pop();
        if (!frame)
            continue;
        emit requestImage();
        qreal currtime = clock->get_time();
        qreal frametime = frame->best_effort_timestamp * av_q2d(codec.timeBase);
        qreal delay = frametime - currtime;
        if (delay<0)
            continue;
        copy_frame(frame, current_frame);
        QVideoFrame videoframe = filter_and_convert(frame);
        videosink->setVideoFrame(videoframe);
        break;
    }
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
void FrameOutput::set_filters_on_currentFrame()
{
    Frame frame = make_shared_frame();
    copy_frame(current_frame, frame);
    QVideoFrame videoframe = filter_and_convert(frame);
    videosink->setVideoFrame(videoframe);
    emit requestImage();
}
QVideoFrame FrameOutput::filter_and_convert(Frame frame)
{
    Frame filtered_frame = filters.applyFilters(frame);
    Frame output_frame = converter.convert(filtered_frame);
    QImage image(output_frame->data[0], codec.context->width, codec.context->height, output_frame->linesize[0], QImage::Format_RGB32);
    return QVideoFrame(image);
}

void FrameOutput::pop_frames_by_time(qint64 time_us)
{
    for(;;){
        if (image_queue.size()==0)
            emit requestImage();
        qint64 frame_time = image_queue.front()->best_effort_timestamp * av_q2d(codec.timeBase) * 1'000'000;
        if (frame_time < time_us)
            image_queue.try_pop();
        else
            return;
        emit requestImage();
    }
}

void FrameOutput::stop_and_clear()
{
    image_queue.clear();
    sleeper.wake();
    std::lock_guard _(mutex);
}


