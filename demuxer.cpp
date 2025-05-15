#include <QDebug>

#include "demuxer.h"
#include "packet.h"

constexpr auto PACKET = "\033[33m[Packet]\033[0m";

Demuxer::Demuxer(AVFormatContext* format_context, Synchronizer *sync, QMutex &formatMutex,qint64 bufferization_time)
    :
    format_context(format_context),
    sync(sync),
    formatMutex(formatMutex),
    bufferization_time(bufferization_time)
{
}

void Demuxer::add_context(int stream_id, IMediaContext* context)
{
    medias[stream_id] = context;
}

void Demuxer::demuxe_packets()
{
    sync->check_pause();
    // qDebug()<<"Max video queue size = "<<video->packetQueue.max_size;
    // qDebug()<<"Max audio queue size = "<<audio->packetQueue.max_size;
    // qDebug()<<"Curr video queue size = "<<video->packetQueue.size();
    // qDebug()<<"Curr audio queue size = "<<audio->packetQueue.size();
    push_packets_to_queues();
}

void Demuxer::push_packets_to_queues()
{
    QMutexLocker _(&formatMutex);
    bool is_buffering = true;
    while(is_buffering)
    {
        Packet packet = make_shared_packet();
        int res = av_read_frame(format_context, packet.get());
        if (res<0)
            return;

        qint64 msduration = packet->duration * av_q2d(packet->time_base) * 1000;
        qint64 curr_duration = sync->get_time();
        if (msduration - curr_duration > bufferization_time)
            is_buffering = false;

        if (medias.contains(packet->stream_index)){
            IMediaContext* media = medias[packet->stream_index];
            media->packetQueue.push(std::move(packet));
            emit media->newPacketArrived();
        }
    }
    //qDebug()<<PACKET<<"Video queue size ="<<video->packetQueue.size();
    //qDebug()<<PACKET<<"Audio queue size ="<<audio->packetQueue.size();
}
