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
#include "queue.h"
#include "frame.h"


class FrameOutput: public QObject
{
    Q_OBJECT
public:
    FrameOutput(Synchronizer*, Codec&, MediaParameters*, qint64);

    void start_output();
    void process_image();
    void process_one_image();
    void set_filters_on_currentFrame();
signals:
    void imageOutputted();
private:
    void copy_frame(Frame source, Frame destination);
    QVideoFrame filter_and_convert_frame(Frame frame);
//////////////////////////////////////////////////
//////////////////////////////////////////////////
public:
    bool abort = false;
    Queue<Frame> image_queue;
private:
    Codec& codec;
    Filters filters;
    ImageConverter converter;
    Frame current_frame = make_shared_frame();

    std::mutex mutex;
    Synchronizer* sync;
    QVideoSink* videosink;
};

#endif // FRAMEOUTPUT_H
