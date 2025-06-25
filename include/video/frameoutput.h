#ifndef FRAMEOUTPUT_H
#define FRAMEOUTPUT_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QWaitCondition>
#include <QMutex>
#include <QPair>

#include "video/filters.h"
#include "video/imageconverter.h"
#include "sync/synchronizer.h"
#include "queue.h"
#include "frame.h"

// struct ImageFrame
// {
//     ImageFrame(QVideoFrame&&,qint64);
//     ImageFrame() = default;
//     ImageFrame(ImageFrame&) = default;
//     ImageFrame(ImageFrame&&) = default;
//     ImageFrame& operator=(ImageFrame&) = default;
//     ImageFrame& operator=(ImageFrame&&) = default;
//     QVideoFrame image;
//     qint64 time;
// };

class FrameOutput: public QObject
{
    Q_OBJECT
public:
    FrameOutput(QVideoSink*, Synchronizer*, qint64 queueSize);
    ~FrameOutput();

    void start_output();
    void set_filters_on_currentFrame();
signals:
    void imageOutputted();
//////////////////////////////////////////////////
//////////////////////////////////////////////////
public:
    //Queue<ImageFrame> imageQueue;
    Queue<Frame> imageQueue;
    Filters* filters;
    ImageConverter* converter;
    AVCodecContext* codec_context;
    qreal timebase;

    QWaitCondition imageReady;
    QMutex queueMutex;
    bool abort = false;
    Frame currentFrame = make_shared_frame();
private:
    QMutex conditionMutex;
    Synchronizer* sync;

    QVideoSink* videosink;
};

#endif // FRAMEOUTPUT_H
