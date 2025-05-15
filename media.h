#ifndef MEDIA_H
#define MEDIA_H

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

class Media : public QObject
{
    Q_OBJECT

public:
    Media();

    void set_file(const QUrl& filename,QVideoSink* videosink);
    void resume_pause();
    void slider_pause();
    void mute_unmute();
    void change_volume(qreal);
    void change_time(qreal);

signals:
    void fileChanged(const QUrl,QVideoSink*);
    void playORpause();
    void sliderPause();
    void muteORunmute();
    void volumeChanged(qreal);
    void timeChanged(qreal);

    void outputTime(qint64,qreal);
    void outputGlobalTime(qint64);

    void brightnessChanged(qreal);
    void contrastChanged(qreal);
    void saturationChanged(qreal);

    void lowChanged(qreal);
    void midChanged(qreal);
    void highChanged(qreal);

// private:
//     void fill_packetQueue();
//     bool push_packet_to_queues();

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
    bool isTemporaryPaused = false;

    QTimer* updateTimer;
};

#endif // MEDIA_H
