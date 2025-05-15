#include "player.h"
#include <QDebug>

Player::Player() {
    mediacontext = new Media();
    mediaThread = new QThread(this);
    mediacontext->moveToThread(mediaThread);
    mediaThread->start();

    connect(mediacontext,&Media::outputGlobalTime,this,&Player::globalTime);
    connect(mediacontext,&Media::outputTime,this,[this](qint64 time,qreal pos){
        // qDebug()<<"curr_time ="<<time;
        // qDebug()<<"pos ="<<pos;
        emit newTime(time);

        m_currentPosition=pos;
        emit currentPositionChanged(m_currentPosition);
    });
    //connect(mediacontext,&MediaContext::moveSliderPosition,this,&Media::sliderPositionChanged);
}

QVideoSink *Player::videoSink() const
{
    return m_videoSink;
}

void Player::setVideoSink(QVideoSink *sink)
{
    if (m_videoSink == sink)
        return;
    m_videoSink = sink;
}

void Player::setFile(const QUrl &filename)
{
    if (mediacontext->video)
        disconnect(connection);
    //emit mediacontext.fileChanged(filename);
    emit mediacontext->fileChanged(filename,m_videoSink);
    emit fileSetted();
}

void Player::output_image(QVideoFrame frame)
{
    //qDebug()<<"\033[32m[Screen]\033[0m Outputing image, size = "<<frame.size();
    m_videoSink->setVideoFrame(frame);
}

void Player::playORpause()
{
    // if (!connection)
    //     connect(mediacontext->video->output, &FrameOutput::imageToOutput, this, &Media::output_image);
    emit mediacontext->playORpause();
}

void Player::muteORunmute()
{
    emit mediacontext->muteORunmute();
}

void Player::volumeChanged(qreal volume)
{
    emit mediacontext->volumeChanged(volume);
}

void Player::timeChanged(qreal time)
{
    emit mediacontext->timeChanged(time);
}

void Player::sliderPause()
{
    emit mediacontext->sliderPause();
}

void Player::add5sec()
{

}

void Player::subtruct5sec()
{

}

void Player::repeatMedia()
{

}

void Player::shuffleMedia()
{

}

void Player::changeBrightness(qreal value)
{
    // from -1 to 1
    qreal brightness_val = value * 2.0 - 1.0;
    emit mediacontext->brightnessChanged(brightness_val);
}

void Player::changeContrast(qreal value)
{
    // from 0 to 2
    qreal contrast_val = value * 2.0;
    emit mediacontext->contrastChanged(contrast_val);
}

void Player::changeSaturation(qreal value)
{
    // from 0 to 3
    qreal saturation_val = value * 3.0;
    emit mediacontext->saturationChanged(saturation_val);
}

void Player::changeLowSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit mediacontext->lowChanged(dB);
}

void Player::changeMidSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit mediacontext->midChanged(dB);
}

void Player::changeHighSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit mediacontext->highChanged(dB);
}


qreal Player::currentPosition() const
{
    return m_currentPosition;
}
