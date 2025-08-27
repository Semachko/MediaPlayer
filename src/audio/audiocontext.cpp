#include <QDebug>
#include <QMediaDevices>

#include "audio/audiocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto SAMPLE = "\033[35m[Sample]\033[0m";

AudioContext::AudioContext(AVStream* stream,  Synchronizer* sync, qreal bufferization_time)
    :
    IMediaContext(stream,10),
    decoder(codec),
    equalizer(codec),
    sync(sync)
{
    // Initiating output audio device FORMAT
    format = QMediaDevices::defaultAudioOutput().preferredFormat();
    qDebug() << "Preferred format:";
    qDebug() << "Sample format:" << format.sampleFormat();
    qDebug() << "Sample rate:" << format.sampleRate();
    qDebug() << "Channel count:" << format.channelCount();
    if (!QMediaDevices::defaultAudioOutput().isFormatSupported(format)) {
        qWarning() << "Error with supporting default audio device!";
        throw;
    }

    // Initiating output audio CONVERTER
    AVChannelLayout outlayout;
    av_channel_layout_default(&outlayout, format.channelCount());
    SampleFormat outputFormat{convert_to_AVFormat(format.sampleFormat()),format.sampleRate(),outlayout};
    converter = new SampleConverter(codec.context, outputFormat);

    // Getting MAX SIZE of audio output buffer
    qint64 bytesPerSample = av_get_bytes_per_sample((AVSampleFormat)outputFormat.format);
    qint64 bytesPerSecond = bytesPerSample * outputFormat.layout.nb_channels * outputFormat.sample_rate;
    maxBufferSize = bytesPerSecond * bufferization_time;

    // Initiating audio output DEVICE
    audioDevice = new AudioIODevice(this);
    audioSink = new QAudioSink(format, this);
    audioSink->setBufferSize(MIN_BUFFER_SIZE);
    audioSink->setVolume(last_volume);
    audioSink->start(audioDevice);
    audioSink->suspend();

    connect(this, &AudioContext::newPacketArrived, this,&AudioContext::process_packet, Qt::QueuedConnection);
    connect(audioDevice, &AudioIODevice::dataReaded, this,&AudioContext::process_packet, Qt::QueuedConnection);
}

AudioContext::~AudioContext()
{
    delete audioSink;
    delete audioDevice;
    //avcodec_free_context(&codec_context);
}

void AudioContext::process_packet()
{
    std::lock_guard _(mutex);
    if (buffer_available() <= 0)
        return;
    Packet packet = packet_queue.try_pop();
    emit requestPacket();
    if (!packet)
        return;
    decode(packet);
    equalizer_and_output();
}

void AudioContext::decode(Packet& packet)
{
    qreal packetTime = packet->pts * av_q2d(timeBase);
    qreal currTime = sync->get_time() / 1000.0;
    qreal diff = currTime - packetTime;
    if (diff > 0.15){
        qDebug()<<"Packet is lating skipping:"<<diff<<"sec";
        return;
    }
    decoder.decode_packet(packet);
}

void AudioContext::equalizer_and_output()
{
    Frame frame = make_shared_frame();
    while (frame = decoder.receive_frame())
    {
        Frame equalized_frame = equalizer.applyEqualizer(frame);
        if (!equalized_frame)
            continue;
        Frame converted_frame = converter->convert(equalized_frame);
        if (!converted_frame)
            continue;
        int size = converted_frame->nb_samples * converted_frame->ch_layout.nb_channels * format.bytesPerSample();
        audioDevice->append(QByteArray((const char*)converted_frame->data[0], size));
        if (audioDevice->bytesAvailable() >= MIN_BUFFER_SIZE && !sync->isPaused)
            emit audioDevice->readyRead();

        av_frame_unref(frame.get());
    }
}

qint64 AudioContext::buffer_available()
{
    qint64 available_bytes = maxBufferSize - audioDevice->bytesAvailable();
    if (available_bytes <= 0 && !sync->isPaused)
        emit audioDevice->readyRead();
    return available_bytes;
}

void AudioContext::set_low(qreal value){
    equalizer.set_low(value);
}
void AudioContext::set_mid(qreal value){
    equalizer.set_mid(value);
}
void AudioContext::set_high(qreal value){
    equalizer.set_high(value);
}
void AudioContext::set_speed(qreal speed){
    equalizer.set_speed(speed);
}
void AudioContext::set_volume(qreal value){
    last_volume=value;
    if (!isMuted)
        audioSink->setVolume(value);
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


