#include <qdebug.>
#include <chrono>

#include "media/decoder.h"

Decoder::Decoder(Codec &codec)
    : codec(codec)
{}
Decoder::~Decoder()
{}

void Decoder::decode_packet(Packet packet)
{
    int result = avcodec_send_packet(codec.context, packet.get());
    if (result < 0)
        qDebug()<<"Error decoding packet: "<<result;
}

QQueue<Frame> Decoder::receive_frames()
{
    QQueue<Frame> queue;
    Frame frame = make_shared_frame();
    while (avcodec_receive_frame(codec.context, frame.get()) == 0){
        queue.enqueue(frame);
        frame = make_shared_frame();
    }
    return queue;
}

void Decoder::drain_decoder()
{
    avcodec_send_packet(codec.context, nullptr);
    qDebug()<<"Decoder drained";
}

void Decoder::clear_decoder()
{
    avcodec_flush_buffers(codec.context);
}

