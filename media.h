#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include "mediacontext.h"

class Media: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink)
    Q_PROPERTY(qreal currentPosition READ currentPosition NOTIFY currentPositionChanged)
    //Q_PROPERTY(qint64 currentTime READ currentTime NOTIFY currentTimeChanged)

public:
    Media();
    QVideoSink* videoSink() const;
    void setVideoSink(QVideoSink* sink);
    qreal currentPosition() const;
    //qint64 currentTime() const;

    Q_INVOKABLE void setFile(const QUrl& filename);
    Q_INVOKABLE void playORpause();
    Q_INVOKABLE void muteORunmute();
    Q_INVOKABLE void volumeChanged(qreal volume);
    Q_INVOKABLE void timeChanged(qreal time);

    Q_INVOKABLE void sliderPause();

    Q_INVOKABLE void add5sec();
    Q_INVOKABLE void subtruct5sec();
    Q_INVOKABLE void repeatMedia();
    Q_INVOKABLE void shuffleMedia();

    Q_INVOKABLE void changeBrightness(qreal value);
    Q_INVOKABLE void changeContrast(qreal value);
    Q_INVOKABLE void changeSaturation(qreal value);

    Q_INVOKABLE void changeLowSounds(qreal value);
    Q_INVOKABLE void changeMidSounds(qreal value);
    Q_INVOKABLE void changeHighSounds(qreal value);
signals:
    Q_SIGNAL void globalTime(qint64 time);
    Q_SIGNAL void newTime(qint64 time);
    Q_SIGNAL void currentPositionChanged(qreal pos);
    Q_SIGNAL void fileSetted();

private:
    void output_image(QVideoFrame frame);
private:
    QVideoSink* m_videoSink;
    MediaContext* mediacontext;
    QThread* mediaThread;
    QMetaObject::Connection connection;
    qreal m_currentPosition;
};

#endif // MEDIA_H
