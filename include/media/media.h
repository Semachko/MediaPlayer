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
#include <QTimer>

#include <atomic>
#include <condition_variable>

#include "video/videocontext.h"
#include "audio/audiocontext.h"
#include "media/demuxer.h"
#include "sync/clock.h"
#include "sync/synchronizer.h"
#include "media/mediaparameters.h"

class Media : public QObject
{
    Q_OBJECT

public:
    Media(MediaParameters* parameters_);
    ~Media();

    void set_file();
    void resume_pause_timer();

    void seeking_pressed(qreal);
    void seeking_released();

    void add_5sec();
    void subtruct_5sec();

signals:
    void subtruct5sec();
    void playORpause();
    void add5sec();

    void seekingPressed(qreal);
    void seekingReleased();
    void endReached();

private:
    qint64 get_real_time_ms();
    void delete_members();
    void seek_time(int64_t);
public:
    AudioContext* audio = nullptr;
    VideoContext* video = nullptr;
    Demuxer* demuxer;
private:
    AVFormatContext* format_context = nullptr;

    QThread* audioThread;
    QThread* videoThread;
    QThread* demuxerThread;

    Synchronizer* sync;
    MediaParameters* params;
    QTimer updateTimer;
    bool isSeeking = false;
};

#endif // MEDIA_H
