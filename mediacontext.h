#ifndef MEDIACONTEXT_H
#define MEDIACONTEXT_H

extern "C" {
#include "libavformat/avformat.h"
#include <libavutil/imgutils.h>
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
}
#include <QString>
#include <QVideoSink>
#include <QUrl>
#include <QMutex>

#include <atomic>
#include <condition_variable>

#include "videocontext.h"
#include "audiocontext.h"
#include "demuxer.h"
#include "clock.h"
#include "synchronizer.h"

class MediaContext : public QObject
{
    Q_OBJECT

public:
    MediaContext();

    void set_file(const QUrl& filename,QVideoSink* videosink);
    void resume_pause();
    void mute_unmute();
    void change_volume(qreal);
    void change_time(qreal);

signals:
    void fileChanged(const QUrl,QVideoSink*);
    void playORpause();
    void muteORunmute();
    void volumeChanged(qreal);
    void timeChanged(qreal);

private:
    void fill_packetQueue();
    bool push_packet_to_queues();

public:
    VideoContext* video = nullptr;
    AudioContext* audio = nullptr;
    Demuxer* demuxer;
private:
    AVFormatContext* format_context = nullptr;

    QVideoSink* videosink;
    QAudioSink* audiosink;

    QThread* audioThread;
    QThread* videoThread;
    QThread* demuxerThread;

    Synchronizer* sync;
    QMutex formatMutex;
};

#endif // MEDIACONTEXT_H
