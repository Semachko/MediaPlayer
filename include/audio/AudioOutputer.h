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
#include "sync/synchronizer.h"

class AudioOutputer : public QIODevice {
    Q_OBJECT
public:
    explicit AudioOutputer (Synchronizer* sync, Codec& codec, SampleFormat format, MediaParameters* params);

    void clear();
    qint64 bytesAvailable() const override;
    void pop_frames_by_time(qint64 time);
protected:
    void push_data_to_buffer();
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
signals:
    void framesReaded();
    void framesPushed();
///////////////////////////////////////////////
///////////////////////////////////////////////
public:
    Queue<Frame> frame_queue{75};
    const qint64 MIN_BUFFER_SIZE = 4092;
private:
    Codec& codec;
    Synchronizer* sync;
    MediaParameters* params;
    SampleConverter converter;
    SampleFormat format;
    Equalizer equalizer;
    mutable std::mutex buff_mutex;
    QByteArray buffer;
};

#endif // AUDIOIODEVICE_H
