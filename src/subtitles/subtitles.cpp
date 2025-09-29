#include "subtitles/subtitles.h"

Subtitles::Subtitles(std::vector<AVStream*>&& streams, Clock* clock_, Demuxer* demuxer_, MediaParameters* par)
    : subtitle_streams(std::move(streams)),
    demuxer(demuxer_),
    clock(clock_),
    params(par)
{
    QStringList subs;
    for (auto stream : subtitle_streams)
    {
        QString sub;
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
            AVDictionaryEntry *language = av_dict_get(stream->metadata, "language", nullptr, 0);
            if (language)
                sub += QString(language->value);
            AVDictionaryEntry *title = av_dict_get(stream->metadata, "title", nullptr, 0);
            if (title){
                if (!sub.isEmpty())
                    sub += " — ";
                sub += title->value;
            }
            if(sub.isEmpty())
                sub = QString::number(stream->id);
            subs.append(sub);
        }
    }
    params->subs->set_subtitles(std::move(subs));
    connect(params->subs, &SubtitleParameters::currentSubChanged, this, &Subtitles::update_current_sub);
}

void Subtitles::update_current_sub()
{
    if (subs_context){
        demuxer->remove_context(subs_context);
        delete subs_context;
        subs_context = nullptr;
    }
    qint64 current_index = params->subs->index;
    subs_context = new SubtitleContext(subtitle_streams[current_index], clock, params);
    demuxer->add_context(subs_context);
    emit subs_context->requestPacket();
}

void Subtitles::clear()
{
    subs_context->clear();
}

void Subtitles::set_new_subtitles()
{

}

Subtitles::~Subtitles()
{
    delete subs_context;
    subs_context = nullptr;
}
