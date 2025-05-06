#ifndef FRAMEOUTPUT_H
#define FRAMEOUTPUT_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QWaitCondition>
#include <QMutex>
#include <QPair>

#include "synchronizer.h"
#include "queue.h"

struct ImageFrame
{
    ImageFrame(QVideoFrame&&,qint64);
    ImageFrame(ImageFrame&) = default;
    ImageFrame(ImageFrame&&) = default;
    ImageFrame& operator=(ImageFrame&) = default;
    ImageFrame& operator=(ImageFrame&&) = default;
    QVideoFrame image;
    qint64 time;
};

class FrameOutput: public QObject
{
    Q_OBJECT
public:
    FrameOutput(Synchronizer*,QVideoSink*);
    void start_output();
signals:
    //void imageToOutput(QVideoFrame frame);
    void imageOutputted();
//////////////////////////////////////////////////
//////////////////////////////////////////////////
public:
    Queue<ImageFrame> imageQueue;
    QWaitCondition imageReady;
    QMutex queueMutex;
private:
    QMutex conditionMutex;
    Synchronizer* sync;

    QVideoSink* videosink;
};

#endif // FRAMEOUTPUT_H
