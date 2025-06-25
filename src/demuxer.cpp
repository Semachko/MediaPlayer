#include <QDebug>

#include "media/demuxer.h"
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
    //sync->check_pause();
    for (auto& [_stream, context] : medias)
        while (!context->packetQueue.is_full()){
            QMutexLocker _(&formatMutex);
            if (!push_packet_to_queues())
                return;
    }
}

bool Demuxer::push_packet_to_queues()
{
    Packet packet = make_shared_packet();
    int res = av_read_frame(format_context, packet.get());
    if (res<0){
        if (res == AVERROR_EOF)
            emit endReached();
        return false;
    }

    if (medias.contains(packet->stream_index)){
        IMediaContext* media = medias[packet->stream_index];
        media->packetQueue.push(std::move(packet));
        emit media->newPacketArrived();
    }
    return true;
}

Demuxer::~Demuxer()
{
}
