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
Player::~Player() {
    mediaThread->quit();
    mediaThread->wait();
    media->deleteLater();
    mediaThread->deleteLater();
    delete params;
}

QString Player::validatePath(const QString& url) {
    if (!QUrl(url).isValid())
        return "";
    if (url.startsWith("file:/"))
        return QUrl(url).toLocalFile();
    return url;
}

void Player::setFile(const QString& url) {
    QString path = validatePath(url);
    if (path.isEmpty())
        return;
    if (isSubtitleFile(path)) {
        QStringList subs(path);
        params->subs->add_subs(subs);
        return;
    }
    QString newFile = playlist.set_new_file(path);
    if (newFile.isEmpty())
        return;
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}
void Player::setFiles(const QStringList& urls) {
    if (urls.isEmpty())
        return;
    if (urls.size() == 1) {
        setFile(urls[0]);
        return;
    }
    QStringList mediaFiles;
    QStringList subtitleFiles;
    for (const QString& url : urls) {
        QString path = validatePath(url);
        if (path.isEmpty())
            continue;
        if (isSubtitleFile(path))
            subtitleFiles << path;
        else if (isMediaFile(path))
            mediaFiles << path;
    }
    if (!mediaFiles.isEmpty()) {
        QString newFile = playlist.set_new_playlist(mediaFiles);
        if (!newFile.isEmpty()) {
            params->file->setPath(newFile);
            params->file->setName(QFileInfo(newFile).fileName());
        }
    }
    if (!subtitleFiles.isEmpty()) {
    }
}

void Player::shuffleMedia() {
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.shuffle_playlist();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::nextMedia() {
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.next_file();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::prevMedia() {
    if (playlist.isEmpty())
        return;
    QString newFile = playlist.prev_file();
    params->file->setPath(newFile);
    params->file->setName(QFileInfo(newFile).fileName());
}

void Player::add5sec() {
    bool expected = false;
    if (media->is_seeking_processing.compare_exchange_strong(expected, true))
        emit media->add5sec();
}
void Player::subtruct5sec() {
    bool expected = false;
    if (media->is_seeking_processing.compare_exchange_strong(expected, true))
        emit media->subtruct5sec();
}
void Player::seekingPressed(qreal timepos) {
    bool expected = false;
    if (media->is_seeking_processing.compare_exchange_strong(expected, true))
        emit media->seekingPressed(timepos);
}

void Player::seekingReleased() { emit media->seekingReleased(); }

bool Player::isSubtitleFile(const QString& filePath) {
    static const QStringList subtitleExts = {
        "srt", "ass", "ssa", "vtt", "sub", "mpl", "smi", "sbv", "jss", "pjs", "aqt", "txt", "rt"};
    QString ext = QFileInfo(filePath).suffix().toLower();
    return subtitleExts.contains(ext);
}
bool Player::isMediaFile(const QString& filePath) {
    static const QStringList mediaExts = {"mp4", "mkv",  "avi", "mov",  "flv", "webm", "ts",  "mpeg",
                                          "mpg", "3gp",  "m4v", "wmv",  "mp3", "aac",  "wav", "flac",
                                          "ogg", "opus", "wma", "alac", "ac3", "dts"};
    QString ext = QFileInfo(filePath).suffix().toLower();
    return mediaExts.contains(ext);
}
