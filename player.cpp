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

void Player::setFile(QUrl filepath, bool isPlaying)
{
    QString newFile = playlist.set_new_file(filepath);
    if(newFile.isEmpty())
        return;
    emit media->fileChanged(newFile,m_videoSink,isPlaying);
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

void Player::shuffleMedia(bool isPlaying)
{
    emit media->set_file(playlist.shuffle_playlist(), m_videoSink,isPlaying);
}
void Player::repeatMedia()
{

}


void Player::prevMedia(bool isPlaying)
{
    emit media->set_file(playlist.prev_file(), m_videoSink,isPlaying);
}
void Player::subtruct5sec()
{
    emit media->subtruct5sec();
}
void Player::playORpause()
{
    emit media->playORpause();
}
void Player::add5sec()
{
    emit media->add5sec();
}
void Player::nextMedia(bool isPlaying)
{
    QString next_file = playlist.next_file();
    qDebug()<<"Setting file:"<<next_file;
    emit media->set_file(next_file, m_videoSink,isPlaying);
}

void Player::changeSpeed(qreal speed)
{
    emit media->speedChanged(speed);
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



qreal Player::currentPosition() const
{
    return m_currentPosition;
}
