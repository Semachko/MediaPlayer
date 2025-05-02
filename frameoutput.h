#ifndef FRAMEOUTPUT_H
#define FRAMEOUTPUT_H

#include <QObject>
#include <QVideoFrame>
#include <QWaitCondition>
#include <QMutex>
#include <QPair>
#include <queue>

#include "synchronizer.h"

struct ImageFrame
{
    ImageFrame(QVideoFrame&&,qint64);
    // ImageFrame(ImageFrame&) = default;
    // ImageFrame(ImageFrame&&) = default;
    // ImageFrame& operator=(ImageFrame&) = default;
    // ImageFrame& operator=(ImageFrame&&) = default;
    QVideoFrame image;
    qint64 time;
};

class FrameOutput: public QObject
{
    Q_OBJECT
public:
    FrameOutput(Synchronizer*);
    void start_output();
    bool is_queue_empty();
    void push_imageQueue(ImageFrame&&);
private:
    ImageFrame pop_imageQueue();
signals:
    void imageToOutput(QVideoFrame frame);
//////////////////////////////////////////////////
//////////////////////////////////////////////////
public:
    QWaitCondition imageReady;
private:
    QMutex conditionMutex;
    QMutex queueMutex;
    Synchronizer* sync;
    std::queue<ImageFrame> imageQueue;
};

#endif // FRAMEOUTPUT_H
