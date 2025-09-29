#include "subtitles/subtitlesoutputer.h"
#include <QThread>

SubtitlesOutputer::SubtitlesOutputer(Clock* clock_, Codec& codec_, MediaParameters* params_)
    : params(params_),
    clock(clock_),
    codec(codec_)
{
    connect(this, &SubtitlesOutputer::outputSubtitles, this, &SubtitlesOutputer::process_subtitles,Qt::QueuedConnection);
    connect(params,&MediaParameters::isPausedChanged,this, [this]{if(!params->isPaused) emit outputSubtitles();},Qt::QueuedConnection);
    if (!params->isPaused)
        emit outputSubtitles();
}
SubtitlesOutputer::~SubtitlesOutputer()
{
    QMetaObject::invokeMethod(params->videoSink, [videoSink = params->videoSink] {
            videoSink->setSubtitleText("");
    }, Qt::QueuedConnection);
}

void SubtitlesOutputer::process_subtitles()
{
    std::lock_guard _(mutex);
    while(!subs_queue.empty()){
        Subtitle sub = subs_queue.try_pop();
        if (sub.text.isEmpty())
            continue;
        emit requestSubtitles();
        qreal delay = sub.start_time - clock->get_time();
        qDebug()<<sub.start_time<<sub.text<<sub.end_time;
        if (delay<-0.2)
            continue;
        sleeper.wait(delay);
        QMetaObject::invokeMethod(params->videoSink, [videoSink = params->videoSink, txt = sub.text] {
            videoSink->setSubtitleText(txt);
        }, Qt::QueuedConnection);
        sleeper.wait(sub.duration);
        if(params->isPaused)
            return;
        QMetaObject::invokeMethod(params->videoSink, [videoSink = params->videoSink] {
            videoSink->setSubtitleText("");
        }, Qt::QueuedConnection);
        if(params->isPaused)
            return;
    }
    if(!params->isPaused)
        emit outputSubtitles();
}

void SubtitlesOutputer::stop_and_clear()
{
    subs_queue.clear();
    while(!mutex.try_lock())
        sleeper.wake();
    mutex.unlock();
}

