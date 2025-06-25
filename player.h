#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "media.h"
#include "playlist.h"

class Player: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink)

    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(qreal currentPosition READ currentPosition NOTIFY currentPositionChanged)
    Q_PROPERTY(qreal timeStep READ timeStep WRITE setTimeStep NOTIFY timeStepChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(bool isMuted READ isMuted WRITE setIsMuted NOTIFY isMutedChanged)
    Q_PROPERTY(bool isPaused READ isPaused WRITE setIsPaused NOTIFY isPausedChanged)
    Q_PROPERTY(bool isRepeating READ isRepeating WRITE setIsRepeating NOTIFY isRepeatingChanged)

    Q_PROPERTY(qreal low READ low WRITE setLow NOTIFY lowChanged)
    Q_PROPERTY(qreal mid READ mid WRITE setMid NOTIFY midChanged)
    Q_PROPERTY(qreal high READ high WRITE setHigh NOTIFY highChanged)
    Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(qreal contrast READ contrast WRITE setContrast NOTIFY contrastChanged)
    Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)

public:
    Player();
    ~Player();

    QVideoSink* videoSink() const;
    void setVideoSink(QVideoSink* sink);

    Q_INVOKABLE void setFile(QUrl filepath);
    Q_INVOKABLE void shuffleMedia(bool isPlaying);
    Q_INVOKABLE void prevMedia(bool isPlaying);
    Q_INVOKABLE void subtruct5sec();
    Q_INVOKABLE void add5sec();
    Q_INVOKABLE void nextMedia(bool isPlaying);
    Q_INVOKABLE void sliderPause(qreal time);
    Q_INVOKABLE void timeChanged(qreal time);

    qreal currentPosition() const;
    qreal volume() const;
    void setVolume(qreal newVolume);
    qreal speed() const;
    void setSpeed(qreal newSpeed);
    bool isMuted() const;
    void setIsMuted(bool newIsMuted);
    bool isPaused() const;
    void setIsPaused(bool newIsPaused);
    bool isRepeating() const;
    void setIsRepeating(bool newIsRepeating);

    qreal low() const;
    void setLow(qreal newLow);
    qreal mid() const;
    void setMid(qreal newMid);
    qreal high() const;
    void setHigh(qreal newHigh);

    qreal brightness() const;
    void setBrightness(qreal newBrightness);
    qreal contrast() const;
    void setContrast(qreal newContrast);
    qreal saturation() const;
    void setSaturation(qreal newSaturation);
    QString filename() const;

signals:
    Q_SIGNAL void globalTime(qint64 time);
    Q_SIGNAL void newTime(qint64 time);
    Q_SIGNAL void paused();

    void filenameChanged();
    void currentPositionChanged();
    void volumeChanged();
    void speedChanged();
    void isMutedChanged();
    void isPausedChanged();
    void isRepeatingChanged();

    void lowChanged();
    void midChanged();
    void highChanged();

    void brightnessChanged();
    void contrastChanged();
    void saturationChanged();

    void timeStepChanged();

private:
    void update_params();
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////

public:
    MediaParameters params;
    void setFilename(const QString &newFilename);

    qreal timeStep() const;
    void setTimeStep(qreal newTimeStep);

private:
    Media* media;
    QThread* mediaThread;
    QVideoSink* m_videoSink;

    QString m_filename = "CHOOSE FILE";
    Playlist playlist;

    qreal m_timeStep = 0.001;
    qreal m_currentPosition;
    qreal m_volume = 0.2;
    qreal m_speed = 1.0;
    bool m_isMuted = false;
    bool m_isPaused = true;
    bool m_isRepeating = false;

    qreal m_low = 0.0;
    qreal m_mid = 0.0;
    qreal m_high = 0.0;

    qreal m_brightness = 0.0;
    qreal m_contrast = 1.0;
    qreal m_saturation = 1.0;
};

#endif // PLAYER_H
