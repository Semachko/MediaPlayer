#ifndef DEMUXER_H
#define DEMUXER_H

extern "C" {
    #include "libavformat/avformat.h"
}

#include <QObject>
#include <vector>
#include <unordered_map>

#include "video/videocontext.h"
#include "audio/audiocontext.h"
#include "sync/synchronizer.h"

class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(AVFormatContext* format_context,Synchronizer* sync);
    ~Demuxer();

    void add_context(IMediaContext* context);
    void demuxe_packets();
    bool push_packet_to_queues();
signals:
    void endReached();
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////
public:
    std::unordered_map<int,IMediaContext*> medias;
    std::mutex mutex;
private:
    AVFormatContext* format_context;
    Synchronizer* sync;
};

#endif // DEMUXER_H
