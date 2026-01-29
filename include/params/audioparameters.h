#pragma once

#include <QObject>

class AudioParameters : public QObject {
        Q_OBJECT
    public:
        explicit AudioParameters(QObject* parent = nullptr) :
                QObject(parent) {}

        Q_INVOKABLE void set_low(qreal val) {
            low = val;
            emit paramsChanged();
        }
        Q_INVOKABLE void set_mid(qreal val) {
            mid = val;
            emit paramsChanged();
        }
        Q_INVOKABLE void set_high(qreal val) {
            high = val;
            emit paramsChanged();
        }

        Q_INVOKABLE void set_volume(qreal val) {
            volume = val;
            emit volumeChanged();
        }
        Q_INVOKABLE void set_isMuted(bool val) {
            isMuted = val;
            emit isMutedChanged();
        }

        std::atomic<qreal> low = 0.0;
        std::atomic<qreal> mid = 0.0;
        std::atomic<qreal> high = 0.0;

        std::atomic<qreal> volume = 0.2;
        std::atomic<bool> isMuted = false;
    signals:
        void paramsChanged();
        void volumeChanged();
        void isMutedChanged();
};