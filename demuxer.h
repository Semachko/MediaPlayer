#ifndef DEMUXER_H
#define DEMUXER_H

extern "C" {
    #include "libavformat/avformat.h"
}

#include <QObject>
#include <vector>
#include <unordered_map>

#include "videocontext.h"
#include "audiocontext.h"
#include "synchronizer.h"

class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(AVFormatContext* format_context,Synchronizer* sync,QMutex& formatMutex);
    ~Demuxer();

    void add_context(int stream_id, IMediaContext* context);
    void demuxe_packets();
    void push_packets_to_queues();
signals:
    void endReached();
private:
    bool is_queues_full();
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////
private:
    QMutex& formatMutex;
    AVFormatContext* format_context;
    Synchronizer* sync;
    std::unordered_map<int,IMediaContext*> medias;
};

#endif // DEMUXER_H
