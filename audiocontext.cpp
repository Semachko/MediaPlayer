#include <QDebug>
#include <QMediaDevices>

#include "audiocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto SAMPLE = "\033[35m[Sample]\033[0m";

AudioContext::AudioContext(AVFormatContext *format_context, Synchronizer* sync) : IMediaContext(16), sync(sync)
{
    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_id<0)
        return;

    AVCodecParameters* codec_parameters = format_context->streams[stream_id]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_parameters->codec_id);

    codec_context = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_context, codec_parameters);
    avcodec_open2(codec_context, codec, nullptr);

    format = QMediaDevices::defaultAudioOutput().preferredFormat();
    qDebug() << "Preferred format:";
    qDebug() << "Sample format:" << format.sampleFormat();
    qDebug() << "Sample rate:" << format.sampleRate();
    qDebug() << "Channel count:" << format.channelCount();
    if (!QMediaDevices::defaultAudioOutput().isFormatSupported(format)) {
        qWarning() << "Error with supporting default audio device!";
        stream_id = -1;
        return;
    }

    equalizer = new Equalizer(codec_context);

    AVChannelLayout outlayout;
    av_channel_layout_default(&outlayout, format.channelCount());
    SampleFormat outputFormat{convert_to_AVFormat(format.sampleFormat()),format.sampleRate(),outlayout};
    converter = new SampleConverter(codec_context,outputFormat);

    audioDevice = new AudioIODevice(sync,this);
    audioSink = new QAudioSink(format, this);
    audioSink->setVolume(last_volume);
    audioSink->start(audioDevice);
    audioSink->suspend();

    connect(this, &AudioContext::newPacketArrived, this,&AudioContext::push_frame_to_buffer, Qt::QueuedConnection);
    connect(audioDevice, &AudioIODevice::dataReaded, this,&AudioContext::push_frame_to_buffer, Qt::QueuedConnection);
}


void AudioContext::push_frame_to_buffer()
{
    sync->check_pause();
    QMutexLocker _(&audioMutex);
    //qDebug()<<DECODING<<"New packet arrived. Popping it";
    // qDebug()<<DECODING<<"Audio device buffer size ="<<audioDevice->buffer.size();
    // qDebug()<<DECODING<<"Audio device buffer size ="<<audioDevice->max_buffer_size;
    // qDebug()<<DECODING<<"Audio device buffer REAL size ="<<audioSink->bufferSize();
    if (audioDevice->buffer.size()>=audioDevice->max_buffer_size){
        //qDebug()<<DECODING<<"Audio device buffer is full, skipping...";
        return;
    }
    if (packetQueue.size()==0){
        emit requestPacket();
        return;
    }

    Packet packet = packetQueue.pop();
    emit requestPacket();
    //qDebug()<<DECODING<<"Audio packet size = "<<packet->size;
    int ret = avcodec_send_packet(codec_context, packet.get());
    if (ret < 0) {
        qDebug()<<"Error sending audio packet: "<<ret;
        return;
    }

    Frame frame = make_shared_frame();
    while (avcodec_receive_frame(codec_context, frame.get()) == 0)
    {
        Frame filtered_frame = equalizer->applyEqualizer(frame);
        Frame coverted_frame = converter->convert(filtered_frame);
        int size = coverted_frame->nb_samples * coverted_frame->ch_layout.nb_channels * format.bytesPerSample();
        audioDevice->appendData(QByteArray((const char*)coverted_frame->data[0], size));
        av_frame_unref(frame.get());
    }
}

void AudioContext::set_low(qreal value)
{
    equalizer->set_low(value);
}

void AudioContext::set_mid(qreal value)
{
    equalizer->set_mid(value);
}

void AudioContext::set_high(qreal value)
{
    equalizer->set_high(value);
}

AVSampleFormat AudioContext::convert_to_AVFormat(QAudioFormat::SampleFormat format)
{
    switch (format) {
    case QAudioFormat::UInt8:   return AV_SAMPLE_FMT_U8;
    case QAudioFormat::Int16:   return AV_SAMPLE_FMT_S16;
    case QAudioFormat::Int32:   return AV_SAMPLE_FMT_S32;
    case QAudioFormat::Float:   return AV_SAMPLE_FMT_FLT;
    default:                    return AV_SAMPLE_FMT_NONE;
    }
}

// qDebug()<<SAMPLE<<"Raw sample:";
// qDebug()<<"format ="<<frame->format;
// qDebug()<<"rate ="<<frame->sample_rate;
// qDebug()<<"bytes per sample="<<av_get_bytes_per_sample(codec_context->sample_fmt) * 8;
// qDebug()<<"channels ="<<frame->ch_layout.nb_channels;


// qDebug()<<SAMPLE<<"Filtered sample:";
// qDebug()<<"format ="<<filtered_frame->format;
// qDebug()<<"rate ="<<filtered_frame->sample_rate;
// qDebug()<<"bytes per sample="<<av_get_bytes_per_sample(static_cast<AVSampleFormat>(filtered_frame->format)) * 8;
// qDebug()<<"channels ="<<filtered_frame->ch_layout.nb_channels;


// qDebug()<<SAMPLE<<"Output sample:";
// qDebug()<<"format ="<<result->format;
// qDebug()<<"rate ="<<result->sample_rate;
// qDebug()<<"bytes per sample="<<av_get_bytes_per_sample(static_cast<AVSampleFormat>(result->format)) * 8;
// qDebug()<<"channels ="<<result->ch_layout.nb_channels;

// qDebug()<<SAMPLE<<"Device sample:";
// qDebug()<<"format ="<<convert_to_AVFormat(format.sampleFormat());
// qDebug()<<"rate ="<<format.sampleRate();
// qDebug()<<"bytes per sample="<<av_get_bytes_per_sample(convert_to_AVFormat(format.sampleFormat())) * 8;
// qDebug()<<"channels ="<<format.channelCount();
