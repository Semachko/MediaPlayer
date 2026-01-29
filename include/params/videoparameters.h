#pragma once

#include <QObject>
#include <QVideoFrame>
#include <QVideoSink>

class VideoParameters : public QObject {
        Q_OBJECT
    public:
        explicit VideoParameters(QObject* parent = nullptr) :
                QObject(parent) {}

        Q_INVOKABLE void set_brightness(qreal val) {
            brightness = val * 2.0 - 1.0;
            emit paramsChanged();
        }
        Q_INVOKABLE void set_contrast(qreal val) {
            contrast = val * 2.0;
            emit paramsChanged();
        }
        Q_INVOKABLE void set_saturation(qreal val) {
            saturation = val * 2.0;
            emit paramsChanged();
        }
        Q_INVOKABLE void set_videoSink(QVideoSink* sink) { previewSink = sink; }
        Q_INVOKABLE void reset_videoSink() { previewSink->setVideoFrame(QVideoFrame()); }
        Q_INVOKABLE void set_preview(qreal seconds) {
            bool expected = false;
            if (is_preview_processing.compare_exchange_strong(expected, true))
                emit setPreview(seconds);
        }

        qreal brightness = 0.0;
        qreal contrast = 1.0;
        qreal saturation = 1.0;
        QVideoSink* previewSink;
        std::atomic<bool> is_preview_processing{false};
    signals:
        void paramsChanged();
        void setPreview(qint64 timepoint);
};