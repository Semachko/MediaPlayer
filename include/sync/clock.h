#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <mutex>

class MediaParameters;

class Clock : public QObject {
        Q_OBJECT
    public:
        Clock(MediaParameters* params);
        void set_time(qreal);
        qreal get_time();
        void pause();
        void resume();
        void set_speed(qreal);

    private:
        std::mutex mutex;
        qreal speed;
        qreal current_time = 0;
        MediaParameters* params;
        QElapsedTimer timer;
};
