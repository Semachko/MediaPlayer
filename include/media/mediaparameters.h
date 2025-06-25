#ifndef MEDIAPARAMETERS_H
#define MEDIAPARAMETERS_H

#include <QString>
#include <QUrl>
struct EqualizerParameters{
    qreal low;
    qreal mid;
    qreal high;
};
struct FiltersParameters{
    qreal brightness;
    qreal contrast;
    qreal saturation;
};


struct MediaParameters{
    QString filepath;
    qreal timePosition;
    qreal volume;
    qreal speed;
    bool isMuted;
    bool isPaused;
    bool isRepeating;
    FiltersParameters filters;
    EqualizerParameters equalizer;
};

#endif // MEDIAPARAMETERS_H
