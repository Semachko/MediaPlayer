#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

#include "media/media.h"
#include "media/playlist.h"

class Player : public QObject {
        Q_OBJECT
    public:
        Player();
        ~Player();
        Q_PROPERTY(MediaParameters* params READ getParams CONSTANT)
        MediaParameters* getParams() const { return params; }

        Q_INVOKABLE void setFile(const QString& filepath);
        Q_INVOKABLE void setFiles(const QStringList& urls);

        Q_INVOKABLE void shuffleMedia();
        Q_INVOKABLE void nextMedia();
        Q_INVOKABLE void prevMedia();

    private:
        bool isSubtitleFile(const QString& filePath);
        bool isMediaFile(const QString& filePath);
        QString validatePath(const QString& path);
        ////////////////////////////////////////////////////////////////////
    private:
        Media* media;
        QThread* mediaThread;
        MediaParameters* params;
        Playlist playlist;
};

#endif  // PLAYER_H
