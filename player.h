#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "media.h"
#include "playlist.h"

class Player: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink)
    Q_PROPERTY(qreal currentPosition READ currentPosition NOTIFY currentPositionChanged)

public:
    Player();
    ~Player();

    QVideoSink* videoSink() const;
    void setVideoSink(QVideoSink* sink);
    qreal currentPosition() const;

    Q_INVOKABLE void setFile(QUrl filepath, bool isPlaying);

    Q_INVOKABLE void changeBrightness(qreal value);
    Q_INVOKABLE void changeContrast(qreal value);
    Q_INVOKABLE void changeSaturation(qreal value);

    Q_INVOKABLE void changeLowSounds(qreal value);
    Q_INVOKABLE void changeMidSounds(qreal value);
    Q_INVOKABLE void changeHighSounds(qreal value);

    Q_INVOKABLE void repeatMedia();
    Q_INVOKABLE void shuffleMedia(bool isPlaying);

    Q_INVOKABLE void prevMedia(bool isPlaying);
    Q_INVOKABLE void subtruct5sec();
    Q_INVOKABLE void playORpause();
    Q_INVOKABLE void add5sec();
    Q_INVOKABLE void nextMedia(bool isPlaying);

    Q_INVOKABLE void changeSpeed(qreal speed);

    Q_INVOKABLE void muteORunmute();
    Q_INVOKABLE void volumeChanged(qreal volume);

    Q_INVOKABLE void timeChanged(qreal time);
    Q_INVOKABLE void sliderPause();


signals:
    Q_SIGNAL void globalTime(qint64 time);
    Q_SIGNAL void newTime(qint64 time);
    Q_SIGNAL void currentPositionChanged(qreal pos);
private:

private:
    Media* media;
    QThread* mediaThread;
    QVideoSink* m_videoSink;
    Playlist playlist;

    qreal m_currentPosition;
};

#endif // PLAYER_H
