#ifndef DEMUXER_H
#define DEMUXER_H

extern "C" {
    #include "libavformat/avformat.h"
}

#include <QObject>
#include <vector>
#include <unordered_map>

#include "video/videocontext.h"
#include "audio/AudioOutputer.h"

class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(AVFormatContext* format_context);
    ~Demuxer();

    void add_context(IMediaContext* context);
    void remove_context(IMediaContext* context);
    void demuxe_packets();
    bool push_packet_to_queues();
    void seek(qint64 time);
    void find_keyframes_to_time(qint64 time);
private:
    bool is_valid_packet(int result);

    ///////////////////////////////////////////////
public:
    std::unordered_map<int,IMediaContext*> medias;
    std::mutex mutex;
private:
    AVFormatContext* format_context;
    bool end_reached = false;
};

#endif // DEMUXER_H
