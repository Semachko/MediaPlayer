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
#include "video/videopreview.h"
#include "audio/audiocontext.h"
#include "sync/clock.h"
#include "media/demuxer.h"
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
private:
    void delete_members();
    void seek_time(qint64);

signals:
    void subtruct5sec();
    void playORpause();
    void add5sec();

    void seekingPressed(qreal);
    void seekingReleased();
    void endReached();
//////////////////////////////////////////////////
//////////////////////////////////////////////////
public:
    std::atomic<bool> is_seeking_processing{false};
private:
    static constexpr qreal bufferization_time = 0.2;
    AudioContext* audio = nullptr;
    VideoContext* video = nullptr;
    VideoPreview* preview = nullptr;
    Demuxer* demuxer = nullptr;
    Clock* clock = nullptr;

    AVFormatContext* format_context = nullptr;

    QThread* audioThread;
    QThread* videoThread;
    QThread* previewThread;
    QThread* demuxerThread;

    MediaParameters* params;
    QTimer updateTimer;
    bool isSeekingPressed = false;
};

#endif // MEDIA_H
