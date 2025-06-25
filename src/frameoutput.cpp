#include "video/frameoutput.h"
#include <QDebug>
#include <QThread>
constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

// ImageFrame::ImageFrame(QVideoFrame&& videoframe, qint64 time)
//     :
//     image(videoframe),
//     time(time)
// {}

FrameOutput::FrameOutput(QVideoSink * videosink, Synchronizer * sync,  qint64 queueSize)
    :
    sync(sync),
    videosink(videosink),
    imageQueue(queueSize)
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
        QMutexLocker q(&conditionMutex);
        //qDebug()<<OUTPUT<<"Checking is image queue empty";
        // qDebug()<<OUTPUT<<"Image queue size ="<<imageQueue.size();
        while(imageQueue.empty()){
            //qDebug()<<OUTPUT<<"Queue is empty, waiting for images";
            imageReady.wait(&conditionMutex);
        }
        //qDebug()<<OUTPUT<<"Queue contain image, outputing";

        // ImageFrame imageFrame;
        // if(!imageQueue.pop(imageFrame))
        //     continue;
        Frame frame = make_shared_frame();
        if(!imageQueue.pop(frame))
            continue;



        av_frame_unref(currentFrame.get());
        currentFrame->format = frame->format;
        currentFrame->width  = frame->width;
        currentFrame->height = frame->height;
        av_frame_get_buffer(currentFrame.get(), 32);
        av_frame_copy(currentFrame.get(),frame.get());
        av_frame_copy_props(currentFrame.get(),frame.get());


        Frame filtered_frame = filters->applyFilters(frame);
        Frame output_frame = converter->convert(filtered_frame);
        QImage image(output_frame->data[0], codec_context->width, codec_context->height, output_frame->linesize[0], QImage::Format_RGB32);
        QVideoFrame videoframe = QVideoFrame(image);
        qint64 frametime = filtered_frame->best_effort_timestamp * 1000 * timebase;
        qint64 delay = frametime - sync->get_time();
        if (delay>0)
            QThread::msleep(delay);
        //qDebug()<<"Delay: "<<delay;
        if (abort)
            return;
        //qDebug()<<"Current time in seconds:"<<sync->get_time()/1000.0;
        videosink->setVideoFrame(videoframe);
        emit imageOutputted();
    }
}

void FrameOutput::set_filters_on_currentFrame()
{
        Frame frame = make_shared_frame();
        frame->format = currentFrame->format;
        frame->width  = currentFrame->width;
        frame->height = currentFrame->height;
        av_frame_get_buffer(frame.get(), 32);
        av_frame_copy(frame.get(),currentFrame.get());
        av_frame_copy_props(frame.get(),currentFrame.get());

        Frame filtered_frame = filters->applyFilters(frame);
        Frame output_frame = converter->convert(filtered_frame);
        QImage image(output_frame->data[0], codec_context->width, codec_context->height, output_frame->linesize[0], QImage::Format_RGB32);
        QVideoFrame videoframe = QVideoFrame(image);
        videosink->setVideoFrame(videoframe);
        emit imageOutputted();
}

