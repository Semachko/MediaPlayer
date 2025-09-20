#include "audio/audiobuffer.h"
#include <Qthread>

AudioBuffer::AudioBuffer(Synchronizer* sync_, Codec& codec_, SampleFormat format_)
    : sync(sync_),
    codec(codec_),
    format(format_)
{
}

void AudioBuffer::push_frame(Frame frame)
{
    if(buffer.size() == 0){
        qreal beginTime = frame->best_effort_timestamp * av_q2d(codec.timeBase);
        time_at_begin = beginTime;
    }
    qint64 dataSize = frame->nb_samples * frame->ch_layout.nb_channels * format.bytes_per_sample;
    buffer.append(QByteArray((const char*)frame->data[0], dataSize));
    //time_at_end = beginTime + (dataSize / bytes_per_ms);
}

qint64 AudioBuffer::sync_copy_to_buffer(char *data, qint64 len)
{
    std::unique_lock ulock(mutex);
    if(buffer.size() == 0)
        return 0;
    qreal curr_time_ms = sync->get_time_ms() / 1000.0;
    qreal delay = time_at_begin - curr_time_ms;
    qDebug()<<"Audio delay:"<<delay;
    // if (delay > 50){
    //     ulock.unlock();
    //     QThread::msleep(delay);
    //     ulock.lock();
    // }
    // else
    // if (delay < -50){
    //     qint64 bytes_to_delete = delay * bytes_per_ms;
    //     buffer.remove(0, bytes_to_delete);
    //     time_at_begin = curr_time_ms;
    // }
    // if (delay < -0.02){
    //     if (frame_size / bytes_per_second <= 0.05)
    //         continue;
    //     Frame processed_frame = equalizer_and_convert(frame);
    //     qint64 new_data_pos = (bytes_per_second / params->speed) * delay;
    //     new_data_pos -= new_data_pos % (processed_frame->ch_layout.nb_channels * format.bytes_per_sample);
    //     qint64 dataSize = processed_frame->nb_samples * processed_frame->ch_layout.nb_channels * format.bytes_per_sample - new_data_pos;
    //     if (dataSize <= 0)
    //         continue;
    //     const char* data = reinterpret_cast<const char*>(processed_frame->data[0]) + new_data_pos;
    //     reading_buffer.append(QByteArray(data, dataSize));
    // }
    len = std::min(buffer.size(),len);
    if (len > 0){
        memcpy(data, buffer.constData(), len);
        buffer.remove(0, len);
        time_at_begin += len / bytes_per_second;
    }
    return len;
}

qint64 AudioBuffer::size() const
{
    std::lock_guard _(mutex);
    return buffer.size();
}

void AudioBuffer::clear()
{
    buffer.clear();
}

void AudioBuffer::calculate_bytes_per_second(qreal speed)
{
    qint64 bytes_per_sample = av_get_bytes_per_sample((AVSampleFormat)format.format);
    bytes_per_second = bytes_per_sample * format.layout.nb_channels * format.sample_rate / speed;
}
