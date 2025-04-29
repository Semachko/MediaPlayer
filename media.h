#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include "mediacontext.h"

class Media: public QObject
{
    Q_OBJECT
public:
    Media();
    void setFile(const QString& filename);
    void play();
    void pause();
    void setTime();
    void add5sec();
    void subtruct5sec();
    void setVolume();
    void muteVolume();
    void repeatMedia();
    void shuffleMedia();
private:
    MediaContext _mediacontext;
};

#endif // MEDIA_H
