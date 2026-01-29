#pragma once

#include "params/audioparameters.h"
#include "params/fileparameters.h"
#include "params/subtitleparameters.h"
#include "params/videoparameters.h"
#include "sync/clock.h"

#include <QDebug>
#include <QObject>

class MediaParameters : public QObject {
        Q_OBJECT
    public:
        explicit MediaParameters(QObject* parent = nullptr) :
                QObject(parent),
                clock(new Clock(this)),
                file(new FileParameters(this)),
                audio(new AudioParameters(this)),
                video(new VideoParameters(this)),
                videoSink(new QVideoSink(this)),
                subs(new SubtitleParameters(this)) {
            connect(subs, &SubtitleParameters::isToggleChanged, this, [this] { videoSink->setSubtitleText(""); });
        }
        Q_PROPERTY(Clock* clock READ getClock CONSTANT)
        Q_PROPERTY(FileParameters* file READ getFile CONSTANT)
        Q_PROPERTY(AudioParameters* audio READ getAudio CONSTANT)
        Q_PROPERTY(VideoParameters* video READ getVideo CONSTANT)
        Q_PROPERTY(SubtitleParameters* subs READ getSubs CONSTANT)
        Clock* getClock() const { return clock; }
        FileParameters* getFile() const { return file; }
        AudioParameters* getAudio() const { return audio; }
        VideoParameters* getVideo() const { return video; }
        SubtitleParameters* getSubs() const { return subs; }

        Q_PROPERTY(QVideoSink* videoSink READ getVideoSink WRITE setVideoSink)
        Q_PROPERTY(bool isPaused READ getIsPaused WRITE setIsPaused NOTIFY isPausedChanged)
        Q_PROPERTY(bool isRepeating READ getIsRepeating WRITE setIsRepeating NOTIFY isRepeatingChanged)

        Q_INVOKABLE void releaseSeeking(qreal time) { emit seekingReleased(time); }
        Q_INVOKABLE void pressSeeking(qreal time) {
            bool expected = false;
            if (isSeeking.compare_exchange_strong(expected, true))
                emit seekingPressed(time);
        }

        QVideoSink* getVideoSink() const { return videoSink; }
        bool getIsPaused() const { return isPaused; }
        bool getIsRepeating() const { return isRepeating; }

        void setVideoSink(QVideoSink* v) { videoSink = v; }
        void setIsPaused(bool v) {
            isPaused = v;
            emit isPausedChanged();
        }
        void setIsRepeating(bool v) {
            isRepeating = v;
            emit isRepeatingChanged();
        }

    signals:
        void seekingPressed(qreal time);
        void seekingReleased(qreal time);
        void isPausedChanged();
        void isRepeatingChanged();

    public:
        std::atomic<bool> isPaused = true;
        std::atomic<bool> isRepeating = false;
        std::atomic<bool> isSeeking = false;

        FileParameters* file;
        AudioParameters* audio;
        VideoParameters* video;
        SubtitleParameters* subs;
        QVideoSink* videoSink;
        Clock* clock;

    private:
};
