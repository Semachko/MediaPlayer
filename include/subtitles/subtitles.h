#ifndef SUBTITLES_H
#define SUBTITLES_H

#include <QObject>
#include "media/demuxer.h"
#include "media/mediaparameters.h"
#include "subtitles/subtitlecontext.h"

class Subtitles : public QObject
{   Q_OBJECT
public:
    Subtitles(std::vector<AVStream*>&& streams, Clock* clock_, Demuxer* demuxer_, MediaParameters* par);
    ~Subtitles();

    void set_new_subtitles();
    void update_current_sub();
    void clear();
private:
    Clock* clock;
    Demuxer* demuxer;
    MediaParameters* params;
    std::vector<AVStream*> subtitle_streams;
    SubtitleContext* subs_context = nullptr;
};

#endif // SUBTITLES_H
