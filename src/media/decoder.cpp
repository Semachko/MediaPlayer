#include <qdebug.>
#include <chrono>
#include <string>

#include "media/decoder.h"

Decoder::Decoder(Codec &codec)
    : codec(codec)
{}
Decoder::~Decoder()
{}

QQueue<Frame> Decoder::decode_packet(Packet packet)
{
    if(packet)
        send_packet(packet);
    return receive_frames();
}
void Decoder::send_packet(Packet packet)
{
    int result = avcodec_send_packet(codec.context, packet.get());
    if (result < 0)
        qDebug()<<"Error decoding packet: "<<result;
}
QQueue<Frame> Decoder::receive_frames()
{
    QQueue<Frame> queue = get_frames();
    if (drained && queue.isEmpty()){
        avcodec_send_packet(codec.context, nullptr);
        return get_frames();
    }
    return queue;
}

Subtitle Decoder::decode_subtitle(Packet packet)
{
    if (!packet)
        return Subtitle();
    qreal start = packet->pts * av_q2d(codec.timeBase);
    qreal duration = packet->duration * av_q2d(codec.timeBase);
    AVSubtitle avsub;
    int got_sub = 0;
    int result = avcodec_decode_subtitle2(codec.context, &avsub, &got_sub, packet.get());
    Subtitle subs(avsub);
    if (result < 0)
        qDebug()<<"Error decoding subtitles: "<<result;
    if (got_sub) {
        for (int i = 0; i < avsub.num_rects; ++i) {
            AVSubtitleRect* r = avsub.rects[i];
            if (r->type == SUBTITLE_TEXT){
                subs.text += r->text;
            }
            else if (r->type == SUBTITLE_ASS){
                const char* ass_text = reinterpret_cast<const char*>(r->data[0]);
                std::string temp = std::string(r->ass);
                QString str = QString::fromUtf8(parseASS(temp).c_str());
                subs.text += str;
            }
        }
    }
    avsubtitle_free(&avsub);
    subs.start_time = start;
    subs.duration = duration;
    subs.end_time = start + duration;
    return subs;
}

QQueue<Frame> Decoder::get_frames()
{
    QQueue<Frame> queue;
    Frame frame = make_shared_frame();
    while (avcodec_receive_frame(codec.context, frame.get()) == 0){
        queue.enqueue(frame);
        frame = make_shared_frame();
    }
    return queue;
}

std::string Decoder::parseASS(std::string& ass_str)
{
    size_t firstComma = ass_str.find(',');
    if (firstComma == std::string::npos)
        return "";
    size_t pos = firstComma;
    int fieldCount = 1;
    while (fieldCount < 8 && pos != std::string::npos) {
        pos = ass_str.find(',', pos + 1);
        fieldCount++;
    }
    if (pos == std::string::npos)
        return "";

    std::string text = ass_str.substr(pos + 1);
    replaceAll(text, "\\N", "\n");
    // std::string cleanText;
    // bool inTag = false;
    // for (char c : text) {
    //     if (c == '{') inTag = true;
    //     else if (c == '}') inTag = false;
    //     else if (!inTag) cleanText += c;
    // }

    return text;
}

void Decoder::replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void Decoder::drain_decoder()
{
    drained  = true;
}

void Decoder::clear_decoder()
{
    drained  = false;
    avcodec_flush_buffers(codec.context);
}

bool Decoder::is_drained()
{
    return drained;
}
