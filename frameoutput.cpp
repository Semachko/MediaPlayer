#include "frameoutput.h"
#include <QDebug>
#include <QThread>
constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

ImageFrame::ImageFrame(QVideoFrame&& videoframe, qint64 time)
    :
    image(videoframe),
    time(time)
{}

FrameOutput::FrameOutput(QVideoSink * videosink, Synchronizer * sync,  qint64 queueSize)
    :
    sync(sync),
    videosink(videosink),
    imageQueue(queueSize)
{}
FrameOutput::~FrameOutput()
{}

void FrameOutput::start_output()
{
    while(!abort)
    {
        sync->check_pause();
        QMutexLocker q(&conditionMutex);
        // qDebug()<<OUTPUT<<"Checking is image queue empty";
        // qDebug()<<OUTPUT<<"Image queue size ="<<imageQueue.size();
        while(imageQueue.empty()){
            // qDebug()<<OUTPUT<<"Queue is empty, waiting for images";
            imageReady.wait(&conditionMutex);
        }
        // qDebug()<<OUTPUT<<"Queue contain image, outputing";
        ImageFrame imageFrame;
        if(!imageQueue.pop(imageFrame))
            continue;

        qint64 delay = imageFrame.time - sync->get_time();
        //qDebug()<<"Delay: "<<delay;
        if (delay>0)
            QThread::msleep(delay);

        if (abort)
            return;
        //qDebug()<<"Current time in seconds:"<<sync->get_time()/1000.0;
        videosink->setVideoFrame(imageFrame.image);
        emit imageOutputted();
    }
}

