#ifndef FRAMEOUTPUT_H
#define FRAMEOUTPUT_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QMutex>
#include <QPair>

#include "media/codec.h"
#include "media/mediaparameters.h"
#include "video/filters.h"
#include "video/imageconverter.h"
#include "sync/synchronizer.h"
#include "sync/threadwaiter.h"
#include "queue.h"
#include "frame.h"


class FrameOutput: public QObject
{
    Q_OBJECT
public:
    FrameOutput(Synchronizer*, Codec&, MediaParameters*, qint64);
    ~FrameOutput();

    void process_image();
    void process_one_image();
    void set_filters_on_currentFrame();
    void pop_frames_by_time(qint64 time);
    void stop_and_clear();
signals:
    void requestImage();
    void outputImage();
private:
    void copy_frame(Frame source, Frame destination);
    QVideoFrame filter_and_convert(Frame frame);
//////////////////////////////////////////////////
//////////////////////////////////////////////////
public:
    Queue<Frame> image_queue;
private:
    Codec& codec;
    Filters filters;
    ImageConverter converter;
    Frame current_frame = make_shared_frame();
    std::mutex mutex;
    Synchronizer* sync;
    ThreadWaiter sleeper;
    QVideoSink* videosink;
    MediaParameters* params;
};

#endif // FRAMEOUTPUT_H
