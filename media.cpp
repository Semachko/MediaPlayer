#include "media.h"
#include <QDebug>

Media::Media() {
    mediacontext = new MediaContext();
    mediaThread = new QThread(this);
    mediacontext->moveToThread(mediaThread);
    mediaThread->start();

    connect(mediacontext,&MediaContext::outputGlobalTime,this,&Media::globalTime);
    connect(mediacontext,&MediaContext::outputTime,this,[this](qint64 time,qreal pos){
        // qDebug()<<"curr_time ="<<time;
        // qDebug()<<"pos ="<<pos;
        emit newTime(time);

        m_currentPosition=pos;
        emit currentPositionChanged(m_currentPosition);
    });
    //connect(mediacontext,&MediaContext::moveSliderPosition,this,&Media::sliderPositionChanged);
}

QVideoSink *Media::videoSink() const
{
    return m_videoSink;
}

void Media::setVideoSink(QVideoSink *sink)
{
    if (m_videoSink == sink)
        return;
    m_videoSink = sink;
}

void Media::setFile(const QUrl &filename)
{
    if (mediacontext->video)
        disconnect(connection);
    //emit mediacontext.fileChanged(filename);
    emit mediacontext->fileChanged(filename,m_videoSink);
}

void Media::output_image(QVideoFrame frame)
{
    //qDebug()<<"\033[32m[Screen]\033[0m Outputing image, size = "<<frame.size();
    m_videoSink->setVideoFrame(frame);
}

void Media::playORpause()
{
    // if (!connection)
    //     connect(mediacontext->video->output, &FrameOutput::imageToOutput, this, &Media::output_image);
    emit mediacontext->playORpause();
}

void Media::muteORunmute()
{
    emit mediacontext->muteORunmute();
}

void Media::volumeChanged(qreal volume)
{
    emit mediacontext->volumeChanged(volume);
}

void Media::timeChanged(qreal time)
{
    emit mediacontext->timeChanged(time);
}

void Media::sliderPause()
{
    emit mediacontext->sliderPause();
}

void Media::add5sec()
{

}

void Media::subtruct5sec()
{

}

void Media::repeatMedia()
{

}

void Media::shuffleMedia()
{

}

void Media::changeBrightness(qreal value)
{
    // from -1 to 1
    qreal brightness_val = value * 2.0 - 1.0;
    emit mediacontext->brightnessChanged(brightness_val);
}

void Media::changeContrast(qreal value)
{
    // from 0 to 2
    qreal contrast_val = value * 2.0;
    emit mediacontext->contrastChanged(contrast_val);
}

void Media::changeSaturation(qreal value)
{
    // from 0 to 3
    qreal saturation_val = value * 3.0;
    emit mediacontext->saturationChanged(saturation_val);
}

void Media::changeLowSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit mediacontext->lowChanged(dB);
}

void Media::changeMidSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit mediacontext->midChanged(dB);
}

void Media::changeHighSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit mediacontext->highChanged(dB);
}


qreal Media::currentPosition() const
{
    return m_currentPosition;
}
