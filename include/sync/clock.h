#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <mutex>

class MediaParameters;

class Clock : public QObject {
        Q_OBJECT
    public:
        Clock(MediaParameters* params);
        Q_PROPERTY(qreal speed WRITE set_speed NOTIFY speedChanged);
        Q_PROPERTY(qreal time WRITE set_time READ get_time NOTIFY timeChanged);

        Q_INVOKABLE void add5sec();
        Q_INVOKABLE void subtruct5sec();

        void set_speed(qreal);
        void set_time(qreal);
        void set_time_without_singaling(qreal);
        qreal get_speed();
        qreal get_time();
        void pause();
        void resume();

    signals:
        void timeChanged();
        void speedChanged();

    private:
        std::mutex mutex;
        std::atomic<qreal> speed = 1.0;
        qreal current_time = 0.0;
        MediaParameters* params;
        QElapsedTimer timer;
};
