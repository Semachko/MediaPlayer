#include "subtitles/subtitlecontext.h"


SubtitleContext::SubtitleContext(AVStream *stream, Clock *clock_, MediaParameters *params)
    :
    IMediaContext(stream, 1), // 1 = one thread required in subtitles decoding
    clock(clock_)
{
    outputer = new SubtitlesOutputer(clock, codec, params);
    outputer->subs_queue.set_full_size(10);
    outputerThread = new QThread();
    outputer->moveToThread(outputerThread);
    outputerThread->start();
    connect(outputer,&SubtitlesOutputer::requestSubtitles, this, &SubtitleContext::process_packet,Qt::QueuedConnection);

    connect(this,&SubtitleContext::newPacketArrived, this, &SubtitleContext::process_packet);
    connect(this,&IMediaContext::endReached, [this]{decoder.drain_decoder(); emit newPacketArrived();});
}

SubtitleContext::~SubtitleContext()
{
    clear();
    outputer->deleteLater();
    outputerThread->quit();
    outputerThread->wait();
    outputerThread->deleteLater();
}

void SubtitleContext::process_packet()
{
    if (buffer_available() <= 0)
        return;
    std::lock_guard _(mutex);
    Packet packet = packet_queue.try_pop();
    if (!decoder.is_drained()){
        emit requestPacket();
    }
    Subtitle sub = decoder.decode_subtitle(packet);
    if (sub.text.isNull())
        return;
    outputer->subs_queue.push(std::move(sub));
}

qint64 SubtitleContext::buffer_available()
{
    qint64 available_size = outputer->subs_queue.get_full_size() - outputer->subs_queue.size();
    return available_size;
}

void SubtitleContext::clear()
{
    std::lock_guard _(mutex);
    packet_queue.clear();
    decoder.clear_decoder();
    outputer->stop_and_clear();
}
