﻿#include "media/player.h"
#include "media/mediaparameters.h"
#include <QDebug>

Player::Player() {
    media = new Media();
    connect(media,&Media::endReached,this,&Player::paused);
    connect(media,&Media::outputGlobalTime,this,&Player::globalTime);
    connect(media,&Media::outputTimeStep,this,[this](qreal step){
        m_timeStep = step;
    });
    connect(media,&Media::outputTime,this,[this](qint64 time,qreal pos){
        m_currentPosition=pos;
        emit newTime(time);
        emit currentPositionChanged();
    });
}

Player::~Player()
{
    media->deleteLater();
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
void Player::update_params()
{
    params.timePosition = m_currentPosition;
    params.volume = m_volume;
    params.speed = m_speed;
    params.isMuted = m_isMuted;
    params.isPaused = m_isPaused;
    params.isRepeating = m_isRepeating;
    params.filters = FiltersParameters{m_brightness,m_contrast,m_saturation};
    params.equalizer = EqualizerParameters{m_low,m_mid,m_high};
}

void Player::setFile(QUrl filepath)
{
    update_params();
    QString newFile = playlist.set_new_file(filepath);
    if(newFile.isEmpty())
        return;
    params.filepath = newFile;
    setFilename(QFileInfo(params.filepath).fileName());
    media->set_file(params,m_videoSink);
}

void Player::shuffleMedia(bool isPlaying){
    if (playlist.isEmpty())
        return;
    update_params();
    params.filepath = playlist.shuffle_playlist();
    setFilename(QFileInfo(params.filepath).fileName());
    media->set_file(params,m_videoSink);
}

void Player::nextMedia(bool isPlaying){
    if (playlist.isEmpty())
        return;
    update_params();
    params.filepath = playlist.next_file();
    setFilename(QFileInfo(params.filepath).fileName());
    media->set_file(params, m_videoSink);
}

void Player::prevMedia(bool isPlaying){
    if (playlist.isEmpty())
        return;
    update_params();
    params.filepath = playlist.prev_file();
    setFilename(QFileInfo(params.filepath).fileName());
    media->set_file(params, m_videoSink);
}

void Player::add5sec(){
    emit media->add5sec();
}
void Player::subtruct5sec(){
    emit media->subtruct5sec();
}



void Player::sliderPause(qreal time){
    emit media->sliderPause(time);
}
void Player::timeChanged(qreal time){
    emit media->timeChanged(time);
}
qreal Player::currentPosition() const{
    return m_currentPosition;
}



qreal Player::volume() const{
    return m_volume;
}
void Player::setVolume(qreal newVolume)
{
    if (qFuzzyCompare(m_volume, newVolume))
        return;
    m_volume = newVolume;

    emit media->volumeChanged(m_volume);
    emit volumeChanged();
}

qreal Player::speed() const{
    return m_speed;
}
void Player::setSpeed(qreal newSpeed)
{
    if (qFuzzyCompare(m_speed, newSpeed))
        return;
    m_speed = newSpeed;
    emit media->speedChanged(m_speed);
    emit speedChanged();
}

bool Player::isMuted() const{
    return m_isMuted;
}
void Player::setIsMuted(bool newIsMuted)
{
    m_isMuted = newIsMuted;
    emit media->muteORunmute();
    emit isMutedChanged();
}

bool Player::isPaused() const{
    return m_isPaused;
}
void Player::setIsPaused(bool newIsPaused)
{
    m_isPaused = newIsPaused;
    emit media->playORpause();
    emit isPausedChanged();
}

bool Player::isRepeating() const{
    return m_isRepeating;
}
void Player::setIsRepeating(bool newIsRepeating)
{
    m_isRepeating = newIsRepeating;
    emit media->repeatingChanged();
    emit isRepeatingChanged();
}



qreal Player::low() const{
    return m_low;
}
void Player::setLow(qreal newLow)
{
    if (qFuzzyCompare(m_low, newLow))
        return;
    m_low = newLow;
    emit media->lowChanged(m_low);
    emit lowChanged();
}

qreal Player::mid() const{
    return m_mid;
}
void Player::setMid(qreal newMid)
{
    if (qFuzzyCompare(m_mid, newMid))
        return;
    m_mid = newMid;
    emit media->midChanged(m_mid);
    emit midChanged();
}

qreal Player::high() const{
    return m_high;
}
void Player::setHigh(qreal newHigh)
{
    if (qFuzzyCompare(m_high, newHigh))
        return;
    m_high = newHigh;
    emit media->highChanged(m_high);
    emit highChanged();
}



qreal Player::brightness() const{
    return m_brightness;
}

void Player::setBrightness(qreal newBrightness)
{
    // from -1 to 1
    m_brightness = newBrightness * 2.0 - 1.0;
    emit media->brightnessChanged(m_brightness);
    emit brightnessChanged();
}

qreal Player::contrast() const{
    return m_contrast;
}
void Player::setContrast(qreal newContrast)
{
    // from 0 to 2
    m_contrast = newContrast * 2.0;
    emit media->contrastChanged(m_contrast);
    emit contrastChanged();
}

qreal Player::saturation() const{
    return m_saturation;
}
void Player::setSaturation(qreal newSaturation)
{
    // from 0 to 2
    m_saturation = newSaturation * 2.0;
    emit media->saturationChanged(m_saturation);
    emit saturationChanged();
}

QString Player::filename() const
{
    return m_filename;
}

void Player::setFilename(const QString &newFilename)
{
    if (m_filename == newFilename)
        return;
    m_filename = newFilename;
    emit filenameChanged();
}

qreal Player::timeStep() const
{
    return m_timeStep;
}

void Player::setTimeStep(qreal newTimeStep)
{
    if (qFuzzyCompare(m_timeStep, newTimeStep))
        return;
    m_timeStep = newTimeStep;
    emit timeStepChanged();
}
