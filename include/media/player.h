#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "media/media.h"
#include "media/playlist.h"

class Player: public QObject
{Q_OBJECT
public:
    Player();
    ~Player();
    Q_PROPERTY(MediaParameters* params READ getParams CONSTANT)
    MediaParameters* getParams() const { return params; }

    Q_INVOKABLE void setFile(QUrl filepath);
    Q_INVOKABLE void shuffleMedia();

    Q_INVOKABLE void nextMedia();
    Q_INVOKABLE void prevMedia();

    Q_INVOKABLE void add5sec();
    Q_INVOKABLE void subtruct5sec();

    Q_INVOKABLE void seekingPressed(qreal time);
    Q_INVOKABLE void seekingReleased();
////////////////////////////////////////////////////////////////////
private:
    Media* media;
    QThread* mediaThread;
    MediaParameters* params;
    Playlist playlist;
};

#endif // PLAYER_H
