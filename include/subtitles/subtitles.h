#ifndef SUBTITLES_H
#define SUBTITLES_H

#include <QThread>

#include "media/demuxer.h"
#include "params/mediaparameters.h"
#include "subtitles/subtitle.h"

class Subtitles : public QObject {
        Q_OBJECT
    public:
        Subtitles(MediaParameters* par, Clock* clock);
        ~Subtitles();
        void add_subs(QString sub_filepath);
        void update_current_sub();
        void clear();

    private:
        QStringList subs_in_ui;
        std::vector<std::unique_ptr<Subtitle>> subs;
        qint64 current_index = -1;
        SubtitlesOutputer* outputer;
        QThread* outputerThread;
        MediaParameters* params;
        Clock* clock;
};

#endif  // SUBTITLES_H
