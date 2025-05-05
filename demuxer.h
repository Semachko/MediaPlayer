#ifndef DEMUXER_H
#define DEMUXER_H

extern "C" {
    #include "libavformat/avformat.h"
}

#include <QObject>

#include "videocontext.h"
#include "audiocontext.h"
#include "synchronizer.h"

class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(AVFormatContext* format_context,Synchronizer* sync,QMutex& formatMutex,VideoContext* video, AudioContext* audio);

    void demuxe_packets();
private:
    bool push_packet_to_queues();
private:
    AVFormatContext* format_context;
    Synchronizer* sync;
    QMutex& formatMutex;

    VideoContext* video = nullptr;
    AudioContext* audio = nullptr;
};

#endif // DEMUXER_H
