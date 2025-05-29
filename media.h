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
#include "mediaparameters.h"

class Media : public QObject
{
    Q_OBJECT

public:
    Media();
    ~Media();

    void set_file(MediaParameters& parameters, QVideoSink* videosink);

    void add_5sec();
    void resume_pause();
    void subtruct_5sec();

    void change_speed(qreal);
    void mute_unmute();
    void change_volume(qreal);

    void change_time(qreal);
    void slider_pause();
    void change_repeating();
signals:
    void fileChanged(QString,QVideoSink*,bool);

    void brightnessChanged(qreal);
    void contrastChanged(qreal);
    void saturationChanged(qreal);
    void lowChanged(qreal);
    void midChanged(qreal);
    void highChanged(qreal);

    void subtruct5sec();
    void playORpause();
    void add5sec();

    void speedChanged(qreal);

    void muteORunmute();
    void volumeChanged(qreal);

    void timeChanged(qreal);

    void sliderPause();
    void outputTime(qint64,qreal);
    void outputGlobalTime(qint64);
    void repeatingChanged();

private:
    void output_one_image();
    void lock_all_mutexes();
    void unlock_all_mutexes();
    void clear_all_buffers();
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
    QMutex formatMutex;

    bool isRepeating = false;
    bool isSliderPause = false;
    QTimer* updateTimer;
};

#endif // MEDIA_H
