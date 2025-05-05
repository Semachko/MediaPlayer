#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include "mediacontext.h"

class Media: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink)
public:
    Media();
    QVideoSink* videoSink() const;
    void setVideoSink(QVideoSink* sink);

    Q_INVOKABLE void setFile(const QUrl& filename,QVideoSink* videosink);
    Q_INVOKABLE void playORpause();
    Q_INVOKABLE void muteORunmute();
    Q_INVOKABLE void volumeChanged(qreal volume);
    Q_INVOKABLE void timeChanged(qreal time);


    Q_INVOKABLE void add5sec();
    Q_INVOKABLE void subtruct5sec();
    Q_INVOKABLE void repeatMedia();
    Q_INVOKABLE void shuffleMedia();
private:
    void output_image(QVideoFrame frame);
private:
    QVideoSink* videosink;
    MediaContext* mediacontext;
    QThread* mediaThread;
    QMetaObject::Connection connection;
};

#endif // MEDIA_H
