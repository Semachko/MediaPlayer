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

VideoContext::VideoContext(AVStream* stream, Synchronizer* sync, MediaParameters* params, qreal bufferization_time)
    :
    IMediaContext(stream, 10),
    decoder(codec),
    sync(sync)
{
    qreal fps = av_q2d(codec.stream->avg_frame_rate);
    maxBufferSize = fps * bufferization_time;

    output = new FrameOutput(sync, codec, params, maxBufferSize);
    outputThread = new QThread();
    output->moveToThread(outputThread);
    outputThread->start();
    connect(output,&FrameOutput::requestImage, this, &VideoContext::process_packet);
    connect(this,&VideoContext::newPacketArrived, this, &VideoContext::process_packet);
    connect(this,&IMediaContext::endReached, [this]{decoder.drain_decoder(); emit newPacketArrived();});
}
VideoContext::~VideoContext()
{
    output->deleteLater();
    outputThread->quit();
    outputThread->wait();
    outputThread->deleteLater();
}

void VideoContext::process_packet()
{
    if (buffer_available() <= 0){
        //qDebug()<<"Buffer is full, returning";
        return;
    }
    std::lock_guard _(mutex);
    Packet packet = packet_queue.try_pop();
    if (!decoder.is_drained()){
        emit requestPacket();
    }
    if (packet)
        decoder.decode_packet(packet);
    get_and_output_frames();
}

void VideoContext::get_and_output_frames()
{
    auto queue = decoder.receive_frames();
    if(queue.empty())
        return;
    output->image_queue.push(std::move(queue));
}

void VideoContext::clear()
{
    std::lock_guard _(mutex);
    packet_queue.clear();
    decoder.clear_decoder();
    output->stop_and_clear();
}

qint64 VideoContext::buffer_available()
{
    qint64 available_size = maxBufferSize - output->image_queue.size();
    if (available_size <= 0)
        return 0;
    return available_size;
}

