#include "player.h"
#include <QDebug>

Player::Player() {
    media = new Media();
    mediaThread = new QThread(this);
    media->moveToThread(mediaThread);
    mediaThread->start();

    connect(media,&Media::outputGlobalTime,this,&Player::globalTime);
    connect(media,&Media::outputTime,this,[this](qint64 time,qreal pos){
        emit newTime(time);
        m_currentPosition=pos;
        emit currentPositionChanged(m_currentPosition);
    });
}

Player::~Player()
{
    mediaThread->quit();
    mediaThread->wait();
    media->deleteLater();
    mediaThread->deleteLater();
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

void Player::setFile(const QUrl &filename, bool isPlaying)
{
    emit media->fileChanged(filename,m_videoSink,isPlaying);
}

void Player::output_image(QVideoFrame frame)
{
    m_videoSink->setVideoFrame(frame);
}

void Player::playORpause()
{
    emit media->playORpause();
}

void Player::muteORunmute()
{
    emit media->muteORunmute();
}

void Player::volumeChanged(qreal volume)
{
    emit media->volumeChanged(volume);
}

void Player::timeChanged(qreal time)
{
    emit media->timeChanged(time);
}

void Player::sliderPause()
{
    emit media->sliderPause();
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
    emit media->brightnessChanged(brightness_val);
}

void Player::changeContrast(qreal value)
{
    // from 0 to 2
    qreal contrast_val = value * 2.0;
    emit media->contrastChanged(contrast_val);
}

void Player::changeSaturation(qreal value)
{
    // from 0 to 3
    qreal saturation_val = value * 3.0;
    emit media->saturationChanged(saturation_val);
}

void Player::changeLowSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit media->lowChanged(dB);
}

void Player::changeMidSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit media->midChanged(dB);
}

void Player::changeHighSounds(qreal value)
{
    // from -12 to 12
    qreal dB = value * 56.0 - 28.0;
    emit media->highChanged(dB);
}


qreal Player::currentPosition() const
{
    return m_currentPosition;
}
