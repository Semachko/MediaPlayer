#include "playlist.h"

#include <QRandomGenerator>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>

Playlist::Playlist() {}

QString Playlist::set_new_file(QUrl& filepath)
{
    if (filepath.isEmpty())
        return QString{};

    QFileInfo fileInfo(filepath.toLocalFile());

    QDir dir(fileInfo.absolutePath());
    const QStringList filters = {"*.mp3", "*.wav", "*.mp4", "*.flac", "*.avi", "*.mkv"};

    mediaFiles = dir.entryInfoList(filters, QDir::Files);
    currentIndex = mediaFiles.indexOf(fileInfo);

    return mediaFiles[currentIndex].absoluteFilePath();
}

QString Playlist::next_file()
{
    if (currentIndex + 1 < mediaFiles.size())
        return mediaFiles[++currentIndex].absoluteFilePath();
    return mediaFiles[0].absoluteFilePath();
}

QString Playlist::prev_file()
{
    if (currentIndex > 0)
        return mediaFiles[--currentIndex].absoluteFilePath();
    return mediaFiles.back().absoluteFilePath();
}

QString Playlist::shuffle_playlist()
{

    std::shuffle(mediaFiles.begin(), mediaFiles.end(), *QRandomGenerator::global());
    return mediaFiles[currentIndex].absoluteFilePath();
}
