#ifndef SUBTITLE_H
#define SUBTITLE_H

#include "media/demuxer.h"
#include "queue.h"
#include "subtitles/subtitlesoutputer.h"
#include "subtitleunit.h"
#include <QString>

class Subtitle : public IMediaContext {
        Q_OBJECT
    public:
        explicit Subtitle(AVStream* stream, std::shared_ptr<AVFormatContext> frm_ctx);
        void process_packet() override;
        qint64 buffer_available() override;
        void clear() override;

        ~Subtitle();
        void disconnect_and_clear();
        void connect_all(SubtitlesOutputer* outputer, Clock* clock);
        ////////////////////////////////////////////////////////
    private:
        std::mutex mutex;
        std::shared_ptr<AVFormatContext> format_context;
        std::unique_ptr<Demuxer> demuxer;
        SubtitlesOutputer* outputer = nullptr;
        QString name;
};

#endif  // SUBTITLE_H