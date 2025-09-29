#include <QDebug>

#include "media/demuxer.h"
#include "packet.h"

constexpr auto PACKET = "\033[33m[Packet]\033[0m";

Demuxer::Demuxer(AVFormatContext* format_context)
    :
    format_context(format_context)
{
}

void Demuxer::add_context(IMediaContext* context)
{
    std::lock_guard _(mutex);
    medias[context->codec.stream->index] = context;
    connect(context,&IMediaContext::requestPacket,this,&Demuxer::demuxe_packets);
}

void Demuxer::remove_context(IMediaContext *context)
{
    std::lock_guard _(mutex);
    medias.erase(context->codec.stream->index);
    disconnect(this, nullptr, context, nullptr);
}

void Demuxer::demuxe_packets()
{
    for (auto& [_stream, context] : medias)
        while (!context->packet_queue.is_full()){
            std::lock_guard _(mutex);
            if (end_reached || !push_packet_to_queues())
                return;
        }
}

bool Demuxer::push_packet_to_queues()
{
    Packet packet = make_shared_packet();
    int res = av_read_frame(format_context, packet.get());
    if (!is_valid_packet(res))
        return false;
    if (medias.contains(packet->stream_index)){
        IMediaContext* media = medias[packet->stream_index];
        media->packet_queue.push(std::move(packet));
        emit media->newPacketArrived();
    }
    return true;
}

void Demuxer::seek(qint64 time)
{
    end_reached = false;
    av_seek_frame(format_context, -1, time, AVSEEK_FLAG_BACKWARD);
}
bool Demuxer::is_valid_packet(int result)
{
    if (result == 0)
        return true;
    if (result == AVERROR_EOF){
        end_reached = true;
        for (auto& [_stream, context] : medias)
            emit context->endReached();
    }
    return false;
}
void Demuxer::find_keyframes_to_time(qint64 time)
{
    std::unordered_map<int,Packet> key_packets;
    for(;;){
        Packet packet = make_shared_packet();
        int res = av_read_frame(format_context, packet.get());
        if (!is_valid_packet(res))
            return;
        if (!(packet->flags & AV_PKT_FLAG_KEY))
            continue;
        if (medias.contains(packet->stream_index)){
            key_packets[packet->stream_index] = packet;
            IMediaContext* media = medias[packet->stream_index];
            int64_t packet_time = av_rescale_q(packet->dts, media->codec.timeBase, AVRational{1, 1000000});
            if (packet_time >= time)
                break;
        }
    }
    for (auto& [index, packet] : key_packets)
        if(packet)
            medias[index]->packet_queue.push(packet);
}
Demuxer::~Demuxer()
{
}
