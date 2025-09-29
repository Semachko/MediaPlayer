#include <QImage>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QtConcurrent>
#include <sstream>

#include "video/videocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto IMAGE = "\033[35m[Image]\033[0m";

VideoContext::VideoContext(AVStream* stream, Clock* clock_, MediaParameters* params, qreal bufferization_time)
    :
    IMediaContext(stream),
    clock(clock_)
{
    qreal fps = av_q2d(codec.stream->avg_frame_rate);
    maxBufferSize = fps * bufferization_time;
    packet_queue.set_full_size(maxBufferSize);

    outputer = new FrameOutput(clock, codec, params, maxBufferSize);
    outputThread = new QThread();
    outputer->moveToThread(outputThread);
    outputThread->start();
    connect(outputer,&FrameOutput::requestImage, this, &VideoContext::process_packet);
    connect(this,&VideoContext::newPacketArrived, this, &VideoContext::process_packet);
    connect(this,&IMediaContext::endReached, [this]{decoder.drain_decoder(); emit newPacketArrived();});
}
VideoContext::~VideoContext()
{
    outputer->deleteLater();
    outputThread->quit();
    outputThread->wait();
    outputThread->deleteLater();
}

void VideoContext::process_packet()
{
    if (buffer_available() <= 0)
        return;
    std::lock_guard _(mutex);
    Packet packet = packet_queue.try_pop();
    if (!decoder.is_drained()){
        emit requestPacket();
    }
    auto queue = decoder.decode_packet(packet);
    if(!queue.empty())
        outputer->image_queue.push(std::move(queue));
}

void VideoContext::clear()
{
    std::lock_guard _(mutex);
    packet_queue.clear();
    decoder.clear_decoder();
    outputer->stop_and_clear();
}

qint64 VideoContext::buffer_available()
{
    qint64 available_size = maxBufferSize - outputer->image_queue.size();
    if (available_size <= 0)
        return 0;
    return available_size;
}

