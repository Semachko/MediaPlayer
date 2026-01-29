#ifndef MEDIA_H
#define MEDIA_H

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
}
#include <QMutex>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QVideoSink>
#include <atomic>
#include <condition_variable>

#include "audio/audiocontext.h"
#include "media/demuxer.h"
#include "params/mediaparameters.h"
#include "subtitles/subtitles.h"
#include "sync/clock.h"
#include "video/videocontext.h"
#include "video/videopreview.h"

class Media : public QObject {
        Q_OBJECT
    public:
        Media(MediaParameters* parameters_);
        ~Media();

        void extracted();
        void set_file();

    private:
        void delete_members();
        void seek_time();
        void seeking_pressed(qreal time);
        void seeking_released(qreal time);

        void initialize_demuxer();
        void try_initialize_audio();
        void try_initialize_video();
        void try_initialize_subtitles();

    signals:
        void playORpause();
        void endReached();
        //////////////////////////////////////////////////
        //////////////////////////////////////////////////

    private:
        static constexpr qreal BUFFERIZATION_TIME = 0.2;
        AudioContext* audio = nullptr;
        VideoContext* video = nullptr;
        VideoPreview* preview = nullptr;
        Subtitles* subs = nullptr;
        Demuxer* demuxer = nullptr;
        Clock* clock = nullptr;

        AVFormatContext* format_context = nullptr;

        QThread* audioThread;
        QThread* videoThread;
        QThread* previewThread;
        QThread* subtitlesThread;
        QThread* demuxerThread;

        MediaParameters* params;
};

#endif  // MEDIA_H
