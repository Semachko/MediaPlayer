#include <QDebug>

#include "demuxer.h"
#include "packet.h"

constexpr auto PACKET = "\033[33m[Packet]\033[0m";

Demuxer::Demuxer(AVFormatContext* format_context, Synchronizer *sync, QMutex &formatMutex)
    :
    format_context(format_context),
    sync(sync),
    formatMutex(formatMutex)
{
}

void Demuxer::add_context(int stream_id, IMediaContext* context)
{
    medias[stream_id] = context;
}

void Demuxer::demuxe_packets()
{
    sync->check_pause();
    push_packets_to_queues();
}

void Demuxer::push_packets_to_queues()
{
    QMutexLocker _(&formatMutex);
    while(!is_queues_full())
    {
        Packet packet = make_shared_packet();
        int res = av_read_frame(format_context, packet.get());
        if (res<0){
            if (res == AVERROR_EOF)
                emit endReached();
            return;
        }

        if (medias.contains(packet->stream_index)){
            IMediaContext* media = medias[packet->stream_index];
            media->packetQueue.push(std::move(packet));
            emit media->newPacketArrived();
        }
    }
}

bool Demuxer::is_queues_full()
{
    for (auto& [_stream, context] : medias)
        if (!context->packetQueue.is_full())
            return false;
    return true;
}


Demuxer::~Demuxer()
{
}
