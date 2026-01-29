#include "subtitles/subtitlesoutputer.h"

#include <QThread>

SubtitlesOutputer::SubtitlesOutputer(Clock* clock_, MediaParameters* params_) :
        params(params_),
        clock(clock_) {
    subs_queue.set_full_size(2);
    connect(
        this, &SubtitlesOutputer::outputSubtitles, this, &SubtitlesOutputer::process_subtitles, Qt::QueuedConnection);
    connect(
        params,
        &MediaParameters::isPausedChanged,
        this,
        [this] {
            if (!params->isPaused && params->subs->is_toggle) {
                emit requestSubtitles();
                emit outputSubtitles();
            }
        },
        Qt::QueuedConnection);
    connect(
        params->subs,
        &SubtitleParameters::isToggleChanged,
        this,
        [this] {
            if (params->subs->is_toggle) {
                emit requestSubtitles();
                emit outputSubtitles();
            }
        },
        Qt::QueuedConnection);

    if (!params->isPaused && params->subs->is_toggle)
        emit outputSubtitles();
}

SubtitlesOutputer::~SubtitlesOutputer() {
    QMetaObject::invokeMethod(
        params->videoSink, [videoSink = params->videoSink] { videoSink->setSubtitleText(""); }, Qt::QueuedConnection);
}

void SubtitlesOutputer::process_subtitles() {
    if (params->isPaused || !params->subs->is_toggle)
        return;
    std::lock_guard _(mutex);
    SubtitleUnit sub = subs_queue.try_pop();
    emit requestSubtitles();
    if (sub.text.isEmpty())
        return;
    qreal delay = sub.start_time - clock->get_time();
    qDebug() << sub.start_time << sub.text << sub.end_time;
    if (delay < -0.2)
        if (abs(delay) > sub.duration)
            return;
        else
            sub.duration += delay;
    sleeper.wait(delay);
    QMetaObject::invokeMethod(
        params->videoSink, [this, txt = sub.text] { params->videoSink->setSubtitleText(txt); }, Qt::QueuedConnection);
    sleeper.wait(sub.duration);
    if (params->isPaused)
        return;
    QMetaObject::invokeMethod(
        params->videoSink, [this] { params->videoSink->setSubtitleText(""); }, Qt::QueuedConnection);
    if (params->isPaused || !params->subs->is_toggle)
        return;
}

void SubtitlesOutputer::stop_and_clear() {
    subs_queue.clear();
    while (!mutex.try_lock())
        sleeper.wake();
    QMetaObject::invokeMethod(
        params->videoSink, [videosink = params->videoSink] { videosink->setSubtitleText(""); }, Qt::QueuedConnection);
    mutex.unlock();
}
