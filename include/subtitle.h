#ifndef SUBTITLE_H
#define SUBTITLE_H

#include <QObject>
#include <memory>
extern "C" {
#include <libavcodec/avcodec.h>
}

struct Subtitle {
    qreal start_time = -1;
    qreal end_time = -1;
    qreal duration = -1;
    QString text;

    Subtitle(){}
    Subtitle(const Subtitle &) = default;
    Subtitle(Subtitle &&) = default;
    Subtitle &operator=(const Subtitle &) = default;
    Subtitle &operator=(Subtitle &&) = default;
    Subtitle(AVSubtitle &sub)
        : start_time(sub.start_display_time / 1000.0),
        end_time(sub.end_display_time / 1000.0),
        duration(end_time - start_time) {}
};


#endif // SUBTITLE_H
