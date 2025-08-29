#ifndef FRAMEOUTPUT_H
#define FRAMEOUTPUT_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QMutex>
#include <QPair>

#include "media/codec.h"
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
    FrameOutput(QVideoSink*, Synchronizer*, Codec&, qint64);
    ~FrameOutput();

    void start_output();
    void process_image();
    void process_one_image();
    void set_filters_on_currentFrame();
signals:
    void imageOutputted();
private:
    void copy_frame(Frame source, Frame destination);
//////////////////////////////////////////////////
//////////////////////////////////////////////////
public:
    //Queue<ImageFrame> image_queue;
    Queue<Frame> image_queue;
    Filters* filters;
    ImageConverter* converter;
    Codec& codec;
    qreal timebase;

    bool abort = false;
    Frame current_frame = make_shared_frame();
private:
    Synchronizer* sync;
    QVideoSink* videosink;
};

#endif // FRAMEOUTPUT_H
