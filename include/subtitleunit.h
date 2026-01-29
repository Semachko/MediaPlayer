#ifndef SUBTITLEUNIT_H
#define SUBTITLEUNIT_H

#include <QObject>
#include <memory>
extern "C" {
#include <libavcodec/avcodec.h>
}

struct SubtitleUnit {
        qreal start_time = -1;
        qreal end_time = -1;
        qreal duration = -1;
        QString text;

        SubtitleUnit() {}
        SubtitleUnit(const SubtitleUnit&) = default;
        SubtitleUnit(SubtitleUnit&&) = default;
        SubtitleUnit& operator=(const SubtitleUnit&) = default;
        SubtitleUnit& operator=(SubtitleUnit&&) = default;
        SubtitleUnit(AVSubtitle& sub) :
                start_time(sub.start_display_time / 1000.0),
                end_time(sub.end_display_time / 1000.0),
                duration(end_time - start_time) {}
};

#endif  // SUBTITLEUNIT_H
