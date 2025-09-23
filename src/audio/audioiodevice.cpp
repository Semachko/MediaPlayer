#include "audio/audiooutputer.h"
#include <qdebug.h>
#include <QThread>

constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

AudioOutputer::AudioOutputer(Clock* clock_, Codec& codec_, SampleFormat format_, MediaParameters* params_, QAudioSink* sink)

    : QIODevice(),
    equalizer(codec, params),
    converter(codec.context, format_),
    out_format(format_),
    params(params_),
    codec(codec_),
    clock(clock_),
    audiosink(sink)
{
    open(QIODevice::ReadOnly);
    bytes_per_second = out_format.layout.nb_channels * out_format.sample_rate * av_get_bytes_per_sample((AVSampleFormat)out_format.format);
}

AudioOutputer::~AudioOutputer()
{
    clear();
}

void AudioOutputer::push_frame(Frame frame)
{
    qint64 frame_size = frame->nb_samples * frame->ch_layout.nb_channels * av_get_bytes_per_sample((AVSampleFormat)frame->format);
    frame_queue.push(frame);
    buffer_size += frame_size;
    if (bytesAvailable() >= MIN_BUFFER_SIZE && !params->isPaused)
        emit readyRead();
}

qint64 AudioOutputer::readData(char *data, qint64 maxlen)
{
    if (maxlen == 0)
        return 0;
    std::lock_guard _(mutex);
    if (reading_buffer.size() > 0){
        int len = std::min(reading_buffer.size(), maxlen);
        memcpy(data, reading_buffer.constData(), len);
        reading_buffer.remove(0, len);
        emit requestFrame();
        return len;
    }
    Frame frame = frame_queue.try_pop();
    emit requestFrame();
    if(!frame)
        return 0;
    qint64 frame_size = frame->nb_samples * frame->ch_layout.nb_channels * av_get_bytes_per_sample((AVSampleFormat)frame->format);
    buffer_size -= frame_size;
    Frame processed_frame = equalizer_and_convert(frame);
    if(!processed_frame)
        return 0;
    qint64 dataSize = processed_frame->nb_samples * processed_frame->ch_layout.nb_channels * out_format.bytes_per_sample;
    reading_buffer.append(QByteArray((const char*)processed_frame->data[0], dataSize));
    int len = std::min(reading_buffer.size(), maxlen);
    if (len > 0){
        memcpy(data, reading_buffer.constData(), len);
        reading_buffer.remove(0, len);
        qreal frametime = processed_frame->best_effort_timestamp * av_q2d(codec.timeBase);
        clock->set_time(frametime);
        return len;
    }
    return 0;
}

Frame AudioOutputer::equalizer_and_convert(Frame frame)
{
    Frame equalized_frame = equalizer.applyEqualizer(frame);
    if (!equalized_frame)
        return nullptr;
    Frame converted_frame = converter.convert(equalized_frame);
    if (!converted_frame)
        return nullptr;
    return converted_frame;
}
qint64 AudioOutputer::bytesAvailable() const
{
    std::lock_guard _(mutex);
    int res = reading_buffer.size()/params->speed + buffer_size + QIODevice::bytesAvailable();
    return res;
}
void AudioOutputer::clear()
{
    std::lock_guard _(mutex);
    reading_buffer.clear();
    frame_queue.clear();
    equalizer.update_equalizer();
    converter.clear();
    buffer_size = 0;
}
qint64 AudioOutputer::writeData(const char *data, qint64 maxSize)
{
    return -1;
}
