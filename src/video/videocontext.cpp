#include <QImage>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QtConcurrent>

#include "video/videocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto IMAGE = "\033[35m[Image]\033[0m";

VideoContext::VideoContext(QVideoSink* videosink, AVStream* stream, Synchronizer* sync, qreal bufferization_time)
    :
    IMediaContext(stream, 10),
    videosink(videosink),
    decoder(codec),
    sync(sync)
{
    qreal fps = av_q2d(codec.stream->avg_frame_rate);
    maxBufferSize = fps * bufferization_time;

    output = new FrameOutput(videosink, sync, codec, maxBufferSize);
    output->filters = new Filters(codec.parameters,timeBase);
    output->converter = new ImageConverter(codec.context);
    output->timebase = av_q2d(timeBase);
    outputThread = new QThread(this);
    output->moveToThread(outputThread);
    outputThread->start();

    connect(output,&FrameOutput::imageOutputted, this, &VideoContext::process_packet);
    QMetaObject::invokeMethod(output,&FrameOutput::start_output, Qt::QueuedConnection);

    connect(this,&VideoContext::newPacketArrived, this, &VideoContext::process_packet);
}

VideoContext::~VideoContext()
{
    outputThread->quit();
    outputThread->wait();
    output->deleteLater();
    outputThread->deleteLater();

    delete imageReady;
}


void VideoContext::process_packet()
{
    std::lock_guard _(mutex);
    if (buffer_available() <= 0)
        return;
    Packet packet = packet_queue.try_pop();
    emit requestPacket();
    if (!packet)
        return;
{
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    auto us = duration_cast<microseconds>(now.time_since_epoch()) % 1000;
    auto s  = duration_cast<seconds>(now.time_since_epoch());

    qDebug().noquote() << "got packet " <<QString("%1:%2:%3")
    .arg((s.count()) % 60, 2, 10, QChar('0'))   // секунды
    .arg(ms.count(), 3, 10, QChar('0'))         // миллисекунды
    .arg(us.count(), 3, 10, QChar('0'));        // микросекунды
}
    decode(packet);
    filter_and_output();
}
void VideoContext::decode(Packet& packet)
{
    qreal packetTime = packet->pts * av_q2d(timeBase);
    qreal currTime = sync->get_time() / 1000.0;
    qreal diff = currTime - packetTime;
    if (diff > 0.15){
        qDebug()<<"diff ="<<diff<<"Lating, skip.";
        return;
    }
    decoder.decode_packet(packet);
}

void VideoContext::filter_and_output()
{
    Frame frame = make_shared_frame();
    while ((frame = decoder.receive_frame())!=nullptr)
    {
        Frame output_frame = make_shared_frame();
        av_frame_move_ref(output_frame.get(), frame.get());
        output->image_queue.push(output_frame);
        {
            using namespace std::chrono;
            auto now = high_resolution_clock::now();
            auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
            auto us = duration_cast<microseconds>(now.time_since_epoch()) % 1000;
            auto s  = duration_cast<seconds>(now.time_since_epoch());

            qDebug().noquote() << "pushed frame " <<QString("%1:%2:%3")
            .arg((s.count()) % 60, 2, 10, QChar('0'))   // секунды
            .arg(ms.count(), 3, 10, QChar('0'))         // миллисекунды
            .arg(us.count(), 3, 10, QChar('0'));        // микросекунды
        }
    }
}

qint64 VideoContext::buffer_available()
{
    qint64 available_size = maxBufferSize - output->image_queue.size();
    if (available_size <= 0)
        return 0;
    return available_size;
}

void VideoContext::set_brightness(qreal value)
{
    output->filters->set_brightness(value);
    if (sync->isPaused)
        output->set_filters_on_currentFrame();
}

void VideoContext::set_contrast(qreal value)
{
    output->filters->set_contrast(value);
    if (sync->isPaused)
        output->set_filters_on_currentFrame();
}

void VideoContext::set_saturation(qreal value)
{
    output->filters->set_saturation(value);
    if (sync->isPaused)
        output->set_filters_on_currentFrame();
}
