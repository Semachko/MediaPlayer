#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QFileInfoList>
#include <QUrl>

class Playlist
{
public:
    Playlist();
    QString set_new_file(QUrl &filepath);
    QString next_file();
    QString prev_file();
    QString shuffle_playlist();
private:
    QFileInfoList mediaFiles;
    int currentIndex = 0;
};

#endif // PLAYLIST_H
