#ifndef FRAMEOUTPUT_H
#define FRAMEOUTPUT_H

#include <QObject>
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
    FrameOutput(Synchronizer*);
    void start_output();
signals:
    void imageToOutput(QVideoFrame frame);
//////////////////////////////////////////////////
//////////////////////////////////////////////////
public:
    quint64 QUEUE_MAX_SIZE = 36;
    Queue<ImageFrame> imageQueue;
    QWaitCondition imageReady;
private:
    QMutex conditionMutex;
    Synchronizer* sync;
};

#endif // FRAMEOUTPUT_H
