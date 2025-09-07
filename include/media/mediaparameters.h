#ifndef MEDIAPARAMETERS_H
#define MEDIAPARAMETERS_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVideoSink>

class AudioParameters : public QObject
{Q_OBJECT
public:
    explicit AudioParameters(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void set_low(qreal val) {low = val; emit paramsChanged();}
    Q_INVOKABLE void set_mid(qreal val) {mid = val; emit paramsChanged();}
    Q_INVOKABLE void set_high(qreal val) {high = val; emit paramsChanged();}

    Q_INVOKABLE void set_volume(qreal val) {volume = val; emit volumeChanged();}
    Q_INVOKABLE void set_isMuted(bool val) {isMuted = val; emit isMutedChanged();}

    std::atomic<qreal> low  = 0.0;
    std::atomic<qreal> mid  = 0.0;
    std::atomic<qreal> high = 0.0;

    std::atomic<qreal> volume = 0.2;
    std::atomic<bool> isMuted = false;
signals:
    void paramsChanged();
    void volumeChanged();
    void isMutedChanged();
};

class VideoParameters : public QObject
{Q_OBJECT
public:
    explicit VideoParameters(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void set_brightness(qreal val) {brightness = val * 2.0 - 1.0; emit paramsChanged();}
    Q_INVOKABLE void set_contrast(qreal val) {contrast = val * 2.0; emit paramsChanged();}
    Q_INVOKABLE void set_saturation(qreal val) {saturation = val * 2.0; emit paramsChanged();}
    Q_INVOKABLE void set_videoSink(QVideoSink* sink) {previewSink = sink;}
    Q_INVOKABLE void set_preview(qint64 timepoint) {
        bool expected = false;
        if (is_preview_processing.compare_exchange_strong(expected, true))
            emit setPreview(timepoint);
    }

    qreal brightness = 0.0;
    qreal contrast = 1.0;
    qreal saturation = 1.0;
    QVideoSink* previewSink;
    std::atomic<bool> is_preview_processing{false};
signals:
    void paramsChanged();
    void setPreview(qint64 timepoint);
};

class FileParameters: public QObject
{Q_OBJECT
public:
    explicit FileParameters(QObject *parent = nullptr) : QObject(parent) {}
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(qint64 globalTime READ getGlobalTime WRITE setGlobalTime NOTIFY globalTimeChanged)
    Q_PROPERTY(qreal timeStep READ getTimeStep WRITE setTimeStep NOTIFY timeStepChanged)
    QString getPath() const {return path;};
    QString getName() const {return name;};
    qint64 getGlobalTime() const {return globalTime;};
    qreal getTimeStep() const {return timeStep;};
    void setPath(const QString &newPath) {path = newPath; emit pathChanged();};
    void setName(const QString &newName) {name = newName; emit nameChanged();};
    void setGlobalTime(const qint64 newTime) {globalTime = newTime; emit globalTimeChanged();};
    void setTimeStep(const qreal newStep) {timeStep = newStep; emit timeStepChanged();};
signals:
    void pathChanged();
    void nameChanged();
    void globalTimeChanged();
    void timeStepChanged();
public:
    QString path;
    QString name = "CHOOSE FILE";
    std::atomic<qint64> globalTime;
    std::atomic<qreal> timeStep = 0.001;
};

class MediaParameters : public QObject
{Q_OBJECT
public:
    explicit MediaParameters(QObject *parent = nullptr)
        : QObject(parent),
        file(new FileParameters(this)),
        audio(new AudioParameters(this)),
        video(new VideoParameters(this)),
        videoSink(new QVideoSink(this)) {}
    Q_PROPERTY(FileParameters* file READ getFile CONSTANT)
    Q_PROPERTY(AudioParameters* audio READ getAudio CONSTANT)
    Q_PROPERTY(VideoParameters* video READ getVideo CONSTANT)
    FileParameters* getFile() const { return file; }
    AudioParameters* getAudio() const { return audio; }
    VideoParameters* getVideo() const { return video; }


    Q_PROPERTY(QVideoSink* videoSink READ getVideoSink WRITE setVideoSink)
    Q_PROPERTY(qint64 currentTime READ getCurrentTime WRITE setCurrentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(qreal speed READ getSpeed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(bool isPaused READ getIsPaused WRITE setIsPaused NOTIFY isPausedChanged)
    Q_PROPERTY(bool isRepeating READ getIsRepeating WRITE setIsRepeating NOTIFY isRepeatingChanged)

    QVideoSink* getVideoSink() const { return videoSink; }
    qint64 getCurrentTime() const { return currentTime; }
    qreal getSpeed() const { return speed; }
    bool getIsPaused() const { return isPaused; }
    bool getIsRepeating() const { return isRepeating; }

    void setVideoSink(QVideoSink* v) { videoSink = v; }
    void setCurrentTime(qint64 v) { currentTime = v; emit currentTimeChanged();}
    void setSpeed(qreal v) { speed = v; emit speedChanged();}
    void setIsPaused(bool v) { isPaused = v; emit isPausedChanged(); }
    void setIsRepeating(bool v) { isRepeating = v; emit isRepeatingChanged();}

signals:
    void currentTimeChanged();
    void speedChanged();
    void isPausedChanged();
    void isRepeatingChanged();

public:
    std::atomic<qint64> currentTime = 0;
    std::atomic<qreal> speed = 1.0;
    std::atomic<bool> isPaused = true;
    std::atomic<bool> isRepeating = false;

    FileParameters* file;
    AudioParameters* audio;
    VideoParameters* video;
    QVideoSink* videoSink;
private:
};


#endif // MEDIAPARAMETERS_H
