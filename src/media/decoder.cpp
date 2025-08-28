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
    auto start = std::chrono::high_resolution_clock::now();

    int result = avcodec_send_packet(codec.context, packet.get());
    if (result < 0)
        qDebug()<<"Error decoding packet: "<<result;

    if (packet->stream_index == 0)
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        qDebug() << "Decoding mcs: " << duration;
    }
}

Frame Decoder::receive_frame()
{
    Frame frame = make_shared_frame();
    int result = avcodec_receive_frame(codec.context, frame.get());
    if (result == 0){
        return frame;
    }
    return Frame{};
}

void Decoder::clear_decoder()
{
    avcodec_flush_buffers(codec.context);
}
