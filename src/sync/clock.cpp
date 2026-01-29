#include "sync/clock.h"
#include "params/mediaparameters.h"

#include <QThread>
#include <algorithm>

Clock::Clock(MediaParameters* params_) :
        QObject(params_),
        params(params_) {
    timer.start();
    if (params->isPaused)
        pause();
    connect(params->file, &FileParameters::pathChanged, this, [this] { set_time(0); }, Qt::DirectConnection);
    connect(
        params,
        &MediaParameters::isPausedChanged,
        this,
        [this] {
            if (params->isPaused)
                pause();
            else
                resume();
        },
        Qt::QueuedConnection);
}
void Clock::add5sec() {
    qreal currtime = get_time() + 5.0;
    if (currtime > params->file->globalTime)
        currtime = params->file->globalTime;
    set_time(currtime);
}
void Clock::subtruct5sec() {
    qreal currtime = get_time() - 5.0;
    if (currtime < 0)
        currtime = 0;
    set_time(currtime);
}

qreal Clock::get_time() {
    std::lock_guard _(mutex);
    if (params->isPaused)
        return std::min(current_time, params->file->globalTime.load());
    qreal seconds = timer.nsecsElapsed() / 1'000'000'000.0;
    current_time += seconds * speed;
    timer.start();
    return std::min(current_time, params->file->globalTime.load());
}
void Clock::set_time(qreal secs) {
    set_time_without_singaling(secs);
    emit timeChanged();
}
void Clock::set_time_without_singaling(qreal secs) {
    std::lock_guard _(mutex);
    current_time = secs;
    timer.start();
}
qreal Clock::get_speed() { return speed.load(); }

void Clock::pause() {
    std::lock_guard _(mutex);
    qreal seconds = timer.nsecsElapsed() / 1'000'000'000.0;
    current_time += seconds * speed;
}
void Clock::resume() {
    std::lock_guard _(mutex);
    timer.start();
}
void Clock::set_speed(qreal new_speed) {
    std::lock_guard _(mutex);
    if (!params->isPaused) {
        qreal seconds = timer.nsecsElapsed() / 1'000'000'000.0;
        current_time += seconds * speed;
        timer.start();
    }
    speed = new_speed;
    emit speedChanged();
}
