#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <media/codec.h>
#include <audio/sampleconverter.h>
#include <sync/synchronizer.h>
#include <frame.h>

#include <QObject>
#include <QByteArray>


class AudioBuffer
{
public:
    AudioBuffer(Synchronizer* sync, Codec& codec, SampleFormat format_);
    void push_frame(Frame frame);
    qint64 sync_copy_to_buffer(char* data, qint64 len);
    qint64 size() const;
    void clear();
    void calculate_bytes_per_second(qreal speed);
public:
    qreal bytes_per_second;
private:
    Synchronizer* sync;
    SampleFormat format;
    Codec& codec;
    mutable std::mutex mutex;

    qreal time_at_begin;
    //qreal time_at_end;
    QByteArray buffer;
};

#endif // AUDIOBUFFER_H
