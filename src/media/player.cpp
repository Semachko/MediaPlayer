#include "media/player.h"
#include "media/mediaparameters.h"
#include <QDebug>

Player::Player() {
    params = new MediaParameters{this};
    media = new Media(params);
    anti_floodseek_timer.setSingleShot(true);
    mediaThread = new QThread(this);
    media->moveToThread(mediaThread);
    mediaThread->start();
}
Player::~Player()
{
    mediaThread->quit();
    mediaThread->wait();
    media->deleteLater();
    mediaThread->deleteLater();
}

void Player::setFile(QUrl filepath){
    QString newFile = playlist.set_new_file(filepath);
    if(newFile.isEmpty())
        return;
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::shuffleMedia(bool isPlaying){
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.shuffle_playlist();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::nextMedia(bool isPlaying){
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.next_file();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::prevMedia(bool isPlaying){
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.prev_file();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::add5sec(){
    emit media->add5sec();
}
void Player::subtruct5sec(){
    emit media->subtruct5sec();
}
void Player::seekingPressed(qreal time){
    if(anti_floodseek_timer.isActive())
        return;
    anti_floodseek_timer.start(50);
    emit media->seekingPressed(time);
}

void Player::seekingReleased()
{
    emit media->seekingReleased();
}


