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


#endif // PACKET_H
