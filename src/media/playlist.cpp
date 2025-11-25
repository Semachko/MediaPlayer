#include "media/playlist.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QRandomGenerator>

Playlist::Playlist() {}

QString Playlist::set_new_file(const QString& filepath) {
    if (filepath.isEmpty())
        return QString{};

    QFileInfo fileInfo(filepath);
    QDir dir(fileInfo.absolutePath());
    mediaFiles = dir.entryInfoList(QDir::Files);
    currentIndex = mediaFiles.indexOf(fileInfo);
    return mediaFiles[currentIndex].absoluteFilePath();
}

QString Playlist::set_new_playlist(const QStringList& playlist) {
    if (playlist.isEmpty())
        return QString{};
    mediaFiles.clear();
    for (const QString& path : playlist)
        mediaFiles << QFileInfo(path);
    currentIndex = 0;
    return mediaFiles[0].absoluteFilePath();
}

QString Playlist::next_file() {
    if (currentIndex + 1 < mediaFiles.size())
        return mediaFiles[++currentIndex].absoluteFilePath();

    currentIndex = 0;
    return mediaFiles[currentIndex].absoluteFilePath();
}

QString Playlist::prev_file() {
    if (currentIndex > 0)
        return mediaFiles[--currentIndex].absoluteFilePath();

    currentIndex = mediaFiles.size() - 1;
    return mediaFiles[currentIndex].absoluteFilePath();
    ;
}

QString Playlist::shuffle_playlist() {

    std::shuffle(mediaFiles.begin(), mediaFiles.end(), *QRandomGenerator::global());
    return mediaFiles[currentIndex].absoluteFilePath();
}

bool Playlist::isEmpty() { return mediaFiles.empty(); }
