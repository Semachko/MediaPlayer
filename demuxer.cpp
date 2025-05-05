#include "demuxer.h"
#include <QDebug>

constexpr auto PACKET = "\033[33m[Packet]\033[0m";

Demuxer::Demuxer(AVFormatContext* format_context, Synchronizer *sync, QMutex &formatMutex, VideoContext* video, AudioContext* audio)
    :
    format_context(format_context),
    sync(sync),
    formatMutex(formatMutex),
    video(video),
    audio(audio)
{
}

void Demuxer::demuxe_packets()
{
    sync->check_pause();
    qDebug()<<"Max video queue size = "<<video->packetQueue.max_size;
    qDebug()<<"Max audio queue size = "<<audio->packetQueue.max_size;
    qDebug()<<"Curr video queue size = "<<video->packetQueue.size();
    qDebug()<<"Curr audio queue size = "<<audio->packetQueue.size();
    while(video->packetQueue.size() + audio->packetQueue.size() < video->packetQueue.max_size + audio->packetQueue.max_size)
        if (!push_packet_to_queues())
            return;
}

bool Demuxer::push_packet_to_queues()
{
    QMutexLocker _(&formatMutex);
    AVPacket *packet = av_packet_alloc();
    int res = av_read_frame(format_context, packet);
    if (res<0)
        return false;
    qDebug()<<"Stream index"<<packet->stream_index;
    if (audio->stream_id>=0 && packet->stream_index == audio->stream_id){
        audio->packetQueue.push(packet);
        qDebug()<<PACKET<<"Pushed packet to audio queue";
        emit audio->newPacketArrived();
        qDebug()<<PACKET<<"Signal emited";
    }
    else if(video->stream_id>=0 && packet->stream_index == video->stream_id){
        video->packetQueue.push(packet);
        qDebug()<<PACKET<<"Pushed packet to video queue";
        emit video->newPacketArrived();
    }
    qDebug()<<PACKET<<"Video queue size ="<<video->packetQueue.size();
    qDebug()<<PACKET<<"Audio queue size ="<<audio->packetQueue.size();
    return true;
}
