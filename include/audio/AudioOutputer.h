#ifndef AUDIOIODEVICE_H
#define AUDIOIODEVICE_H

#include <QIODevice>
#include <QByteArray>
#include <QWaitCondition>

#include "queue.h"
#include "frame.h"
#include "media/mediaparameters.h"
#include "audio/equalizer.h"
#include "audio/sampleconverter.h"
#include "sync/clock.h"

class AudioOutputer : public QIODevice {
    Q_OBJECT
public:
    explicit AudioOutputer (Clock* clock, Codec& codec, SampleFormat format, MediaParameters* params, QAudioSink* sink);
    ~AudioOutputer();

    void clear();
    qint64 bytesAvailable() const override;
    void push_frame(Frame frame);
protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
    Frame equalizer_and_convert(Frame frame);
signals:
    void requestFrame();
    void framesPushed();
///////////////////////////////////////////////
///////////////////////////////////////////////
public:
    const qint64 MIN_BUFFER_SIZE = 4092;
    std::atomic<qint64> buffer_size = 0;
private:
    QAudioSink* audiosink;
    mutable std::mutex mutex;
    qint64 bytes_per_second;
    QByteArray reading_buffer;

    Clock* clock;
    MediaParameters* params;
    Codec& codec;
    SampleFormat out_format;
    Equalizer equalizer;
    SampleConverter converter;
    Queue<Frame> frame_queue;
};

#endif // AUDIOIODEVICE_H
