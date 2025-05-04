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
        qDebug()<<OUTPUT<<"Checking is image queue empty";
        qDebug()<<OUTPUT<<"Image queue size ="<<imageQueue.size();
        while(imageQueue.empty()){
            qDebug()<<OUTPUT<<"Queue is empty, waiting for images";
            imageReady.wait(&conditionMutex);
        }
        if (imageQueue.empty())
             continue;

        qDebug()<<OUTPUT<<"Queue contain image, outputing";
        ImageFrame imageFrame = imageQueue.pop();

        qint64 delay = imageFrame.time - sync->get_time();
        // qDebug()<<"Video time = "<<imageFrame.time;
        // qDebug()<<"Clock time = "<<sync->get_time();
        qDebug()<<"Delay: "<<delay;
        if (delay>0)
            QThread::msleep(delay);

        qDebug()<<OUTPUT<<"Signaling to output image";
        emit imageToOutput(imageFrame.image);
    }
}

