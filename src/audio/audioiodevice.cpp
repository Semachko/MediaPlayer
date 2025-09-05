#include "audio/audiooutputer.h"
#include <qdebug.h>

constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

AudioOutputer::AudioOutputer(Synchronizer* sync, Codec& codec, SampleFormat format_, MediaParameters* params_)

    : QIODevice(),
    format(format_),
    params(params_),
    converter(codec.context, format_),
    equalizer(codec, params),
    sync(sync)
{
    open(QIODevice::ReadOnly);
    connect(this, &AudioOutputer::framesPushed,this,&AudioOutputer::push_data_to_buffer);
}

void AudioOutputer::push_data_to_buffer()
{
    while(!frame_queue.empty())
    {
        Frame frame = frame_queue.try_pop();
        if(!frame)
            continue;
        Frame equalized_frame = equalizer.applyEqualizer(frame);
        if (!equalized_frame)
            continue;
        Frame converted_frame = converter.convert(equalized_frame);
        if (!converted_frame)
            continue;
        int size = converted_frame->nb_samples * converted_frame->ch_layout.nb_channels * format.bytes_per_sample;
        {
            std::lock_guard _(buff_mutex);
            buffer.append(QByteArray((const char*)converted_frame->data[0], size));
        }
        if (bytesAvailable() >= MIN_BUFFER_SIZE && !params->isPaused)
            emit readyRead();
    }

}
qint64 AudioOutputer::readData(char *data, qint64 maxlen)
{
    std::lock_guard _(buff_mutex);
    qint64 len = qMin(maxlen, (qint64)buffer.size());
    if (len > 0) {
        memcpy(data, buffer.constData(), len);
        buffer.remove(0, len);
        emit framesReaded();
    }
    return len;
}
qint64 AudioOutputer::bytesAvailable() const
{
    std::lock_guard _(buff_mutex);
    return buffer.size() + QIODevice::bytesAvailable();
}
void AudioOutputer::clear()
{
    std::lock_guard _(buff_mutex);
    buffer.clear();
}
qint64 AudioOutputer::writeData(const char *data, qint64 maxSize)
{
    return -1;
}
