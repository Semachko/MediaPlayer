#include <qdebug.>

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

Frame Decoder::receive_frame()
{
    Frame frame = make_shared_frame();
    int result = avcodec_receive_frame(codec.context, frame.get());
    if (result == 0){
        return frame;
    }
    qDebug()<<"Error receiving frame: "<<result;
    return Frame{};
}

void Decoder::clear_decoder()
{
    avcodec_flush_buffers(codec.context);
}
