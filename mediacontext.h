#ifndef MEDIACONTEXT_H
#define MEDIACONTEXT_H

#include <QString>
#include <QVideoSink>
#include <QUrl>


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


signals:
    void videoSinkChanged();

private:
    void processMedia();

private:
    AVFormatContext* format_context = nullptr;
    VideoContext* video = nullptr;
    AudioContext* audio = nullptr;

    std::atomic<bool> is_paused = true;
    std::mutex mtx;
    std::condition_variable cv;

    QVideoSink* videosink;
    QAudioSink* audiosink;

    QThread* audioThread;
    QThread* videoThread;
    Synchronizer* sync;
};

#endif // MEDIACONTEXT_H
