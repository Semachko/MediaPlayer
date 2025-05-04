#ifndef MEDIACONTEXT_H
#define MEDIACONTEXT_H

#include <QString>
#include <QVideoSink>
#include <QUrl>
#include <QMutex>

#include <atomic>
#include <condition_variable>

extern "C" {
#include "libavformat/avformat.h"
#include <libavutil/imgutils.h>
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
}

#include "audiocontext.h"
#include "videocontext.h"
#include "masterclock.h"
#include "synchronizer.h"

class MediaContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)

public:
    MediaContext();
    QVideoSink* videoSink() const;
    void setVideoSink(QVideoSink* sink);

    Q_INVOKABLE void setFile(const QUrl& filename);
    Q_INVOKABLE void playORpause();
    Q_INVOKABLE void volumeChanged(qreal);
    Q_INVOKABLE void muteORunmute();
    Q_INVOKABLE void timeChanged(qreal);

private:
    void fill_videoPacketQueue();
    void fill_audioPacketQueue();
    void fill_packetQueue();
    bool push_packet_to_queues();
signals:
    void videoSinkChanged();
private:
    AVFormatContext* format_context = nullptr;
    VideoContext* video = nullptr;
    AudioContext* audio = nullptr;

    QVideoSink* videosink;
    QAudioSink* audiosink;

    QThread* audioThread;
    QThread* videoThread;

    Synchronizer* sync;
    QMutex formatMutex;

    quint64 QUEUE_MAX_SIZE = 20;
};

#endif // MEDIACONTEXT_H
