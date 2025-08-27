#ifndef DECODER_H
#define DECODER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <mutex>
#include "codec.h"
#include "packet.h"
#include "frame.h"

class Decoder
{
public:
    Decoder(Codec& codec);
    ~Decoder();

    void decode_packet(Packet packet);
    Frame receive_frame();
    void clear_decoder();

public:
    Codec& codec;
};

#endif // DECODER_H
