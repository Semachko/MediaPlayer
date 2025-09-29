#include "media/player.h"
#include "media/mediaparameters.h"
#include <QDebug>
#include <QRegularExpression>

Player::Player() {
    params = new MediaParameters{this};
    media = new Media(params);
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
    delete params;
}

void Player::setFile(QUrl filepath){
    QString newFile = playlist.set_new_file(filepath);
    if(newFile.isEmpty())
        return;
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::shuffleMedia(){
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.shuffle_playlist();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::nextMedia(){
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.next_file();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::prevMedia(){
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.prev_file();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::add5sec(){
    bool expected = false;
    if (media->is_seeking_processing.compare_exchange_strong(expected, true))
        emit media->add5sec();
}
void Player::subtruct5sec(){
    bool expected = false;
    if (media->is_seeking_processing.compare_exchange_strong(expected, true))
        emit media->subtruct5sec();
}
void Player::seekingPressed(qreal timepos){
    bool expected = false;
    if (media->is_seeking_processing.compare_exchange_strong(expected, true))
        emit media->seekingPressed(timepos);
}

void Player::seekingReleased()
{
    emit media->seekingReleased();
}


