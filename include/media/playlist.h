#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QFileInfoList>
#include <QUrl>

class Playlist {
    public:
        Playlist();
        QString set_new_file(const QString& filepath);
        QString set_new_playlist(const QStringList& playlist);
        QString next_file();
        QString prev_file();
        QString shuffle_playlist();
        bool isEmpty();

    private:
        QFileInfoList mediaFiles;
        int currentIndex = 0;
};

#endif  // PLAYLIST_H
