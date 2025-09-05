#ifndef DECODER_H
#define DECODER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <QQueue>
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
    QQueue<Frame> receive_frames();
    void clear_decoder();
    void drain_decoder();
    bool is_drained();
public:
    Codec& codec;

private:
    QQueue<Frame> get_frames();
private:
    bool drained = false;
};

#endif // DECODER_H
