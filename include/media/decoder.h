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
#include "subtitle.h"

class Decoder
{
public:
    Decoder(Codec& codec);
    ~Decoder();

    QQueue<Frame> decode_packet(Packet packet);
    Subtitle decode_subtitle(Packet packet);
    void send_packet(Packet packet);
    QQueue<Frame> receive_frames();
    void clear_decoder();
    void drain_decoder();
    bool is_drained();
private:
    QQueue<Frame> get_frames();
    std::string parseASS(std::string& ass_str);
    void replaceAll(std::string &str, const std::string &from, const std::string &to);
/////////////////////////////////////////////
public:
    Codec& codec;
private:
    bool drained = false;
};

#endif // DECODER_H
