#ifndef PACKET_H
#define PACKET_H


#include <memory>
extern "C" {
#include "libavcodec/packet.h"
}

using Packet = std::shared_ptr<AVPacket>;

inline Packet make_shared_packet() {
    return Packet(av_packet_alloc(), [](AVPacket* f){ av_packet_free(&f); });
}

// class Packet
// {
// public:
//     AVPacket* pkt;

// public:
//     Packet() {
//         pkt = av_packet_alloc();
//     }
//     ~Packet() {
//         av_packet_free(&pkt);
//     }

//     Packet(const Packet&) = delete;
//     Packet& operator=(const Packet&) = delete;

//     Packet(Packet&& other) noexcept {
//         pkt = other.pkt;
//         other.pkt = nullptr;
//     }
//     Packet& operator=(Packet&& other) noexcept {
//         if (this != &other) {
//             av_packet_free(&pkt);
//             pkt = other.pkt;
//             other.pkt = nullptr;
//         }
//         return *this;
//     }

//     AVPacket* operator->() { return pkt; }
//     AVPacket* get() const { return pkt; }
// };


#endif // PACKET_H
