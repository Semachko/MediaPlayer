#include "frameoutput.h"
#include <QDebug>
#include <QThread>

ImageFrame::ImageFrame(QVideoFrame&& videoframe, qint64 time)
    : image(videoframe),
    time(time)
{}

FrameOutput::FrameOutput(Synchronizer * sync)
    : sync(sync)
{}
constexpr auto OUTPUT = "\033[34m[Output]\033[0m";
void FrameOutput::start_output()
{
    while(true)
    {
        sync->check_pause();

        QMutexLocker locker(&conditionMutex);
        //qDebug()<<OUTPUT<<"Checking is image queue empty";
        while(imageQueue.empty()){
            //qDebug()<<OUTPUT<<"Queue is empty, waiting for images";
            imageReady.wait(&conditionMutex);
        }
        //qDebug()<<OUTPUT<<"Queue contain image, outputing";
        ImageFrame imageFrame = pop_imageQueue();
        qint64 delay = imageFrame.time - sync->get_time();
        // qDebug()<<"Video time = "<<imageFrame.time;
        // qDebug()<<"Clock time = "<<sync->get_time();
        qDebug()<<"Delay: "<<delay;
        if (delay>0)
            QThread::msleep(delay);

        //qDebug()<<"\033[34m[Output]\033[0m Signaling to output image";
        emit imageToOutput(imageFrame.image);
    }
}

void FrameOutput::push_imageQueue(ImageFrame && imageFrame)
{
    QMutexLocker locker(&queueMutex);
    imageQueue.push(std::move(imageFrame));
}

bool FrameOutput::is_queue_empty()
{
    QMutexLocker locker(&queueMutex);
    return imageQueue.empty();
}

ImageFrame FrameOutput::pop_imageQueue()
{
    QMutexLocker locker(&queueMutex);
    ImageFrame temp = imageQueue.front();
    imageQueue.pop();
    return temp;
}

