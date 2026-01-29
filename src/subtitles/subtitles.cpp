#include "subtitles/subtitles.h"

Subtitles::Subtitles(MediaParameters* par, Clock* clock_) :
        params(par),
        clock(clock_) {
    outputer = new SubtitlesOutputer(clock, params);
    outputerThread = new QThread();
    outputer->moveToThread(outputerThread);
    outputerThread->start();
    connect(params->subs, &SubtitleParameters::currentSubChanged, this, &Subtitles::update_current_sub);
}

void Subtitles::add_subs(QString sub_filepath) {
    AVFormatContext* format_context = nullptr;
    avformat_open_input(&format_context, sub_filepath.toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);
    auto format_context_ptr = std::shared_ptr<AVFormatContext>(format_context, [](AVFormatContext* ctx) {
        if (ctx)
            avformat_close_input(&ctx);
    });

    for (unsigned int i = 0; i < format_context_ptr->nb_streams; ++i) {
        AVStream* stream = format_context_ptr->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
            QString sub;
            AVDictionaryEntry* language = av_dict_get(stream->metadata, "language", nullptr, 0);
            if (language)
                sub += QString(language->value);
            AVDictionaryEntry* title = av_dict_get(stream->metadata, "title", nullptr, 0);
            if (title) {
                if (!sub.isEmpty())
                    sub += " — ";
                sub += title->value;
            }
            if (sub.isEmpty())
                sub = QString::number(stream->id);
            subs_in_ui.append(sub);
            subs.emplace_back(std::make_unique<Subtitle>(stream, format_context_ptr));
        }
    }
    if (!subs_in_ui.empty())
        params->subs->set_subtitles(subs_in_ui);
}

void Subtitles::update_current_sub() {
    if (params->subs->index == -1)
        return;
    if (current_index >= 0) {
        Subtitle* old_sub = subs[current_index].get();
        old_sub->disconnect_and_clear();
        outputer->stop_and_clear();
    }
    current_index = params->subs->index;
    Subtitle* new_sub = subs[current_index].get();
    new_sub->connect_all(outputer, clock);
}

void Subtitles::clear() {}

Subtitles::~Subtitles() {
    outputer->stop_and_clear();
    outputerThread->quit();
    outputerThread->wait();
    delete outputerThread;
    delete outputer;
}
