#ifndef PACKET_H
#define PACKET_H

extern "C" {
    #include "libavcodec/avcodec.h"
}

class Packet
{
public:
    AVPacket* pkt;

public:
    Packet() {
        pkt = av_packet_alloc();
    }
    ~Packet() {
        av_packet_free(&pkt);
    }

    Packet(const Packet&) = delete;
    Packet& operator=(const Packet&) = delete;

    Packet(Packet&& other) noexcept {
        pkt = other.pkt;
        other.pkt = nullptr;
    }
    Packet& operator=(Packet&& other) noexcept {
        if (this != &other) {
            av_packet_free(&pkt);
            pkt = other.pkt;
            other.pkt = nullptr;
        }
        return *this;
    }

    AVPacket* operator->() { return pkt; }
    AVPacket* get() const { return pkt; }
};


#endif // PACKET_H
