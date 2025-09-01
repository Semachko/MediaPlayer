#include <QDebug>
#include <QMediaDevices>

#include "audio/audiocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto SAMPLE = "\033[35m[Sample]\033[0m";

AudioContext::AudioContext(AVStream* stream,  Synchronizer* sync, MediaParameters* params_, qreal bufferization_time)
    :
    IMediaContext(stream,10),
    params(params_),
    decoder(codec),
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

    // Initiating output CONVERTER and OUTPUTER
    AVChannelLayout outlayout;
    av_channel_layout_default(&outlayout, format.channelCount());
    SampleFormat outputFormat{convert_to_AVFormat(format.sampleFormat()),format.sampleRate(),format.bytesPerSample(),outlayout};
    audio_outputer = new AudioOutputer(sync, codec, outputFormat, params);
    audioSink = new QAudioSink(format, this);
    audioSink->setBufferSize(audio_outputer->MIN_BUFFER_SIZE);
    audioSink->setVolume(last_volume);
    audioSink->start(audio_outputer);
    audioSink->suspend();

    // Getting MAX SIZE of audio output buffer
    qint64 bytesPerSample = av_get_bytes_per_sample((AVSampleFormat)outputFormat.format);
    qint64 bytesPerSecond = bytesPerSample * outputFormat.layout.nb_channels * outputFormat.sample_rate;
    maxBufferSize = bytesPerSecond * bufferization_time;

    connect(params, &MediaParameters::isPausedChanged, this,&AudioContext::pause_changed);
    connect(params->audio, &AudioParameters::isMutedChanged,this,&AudioContext::mute_unmute);
    connect(params->audio, &AudioParameters::volumeChanged,this,&AudioContext::set_volume);

    connect(this, &AudioContext::newPacketArrived, this,&AudioContext::process_packet, Qt::QueuedConnection);
    connect(audio_outputer, &AudioOutputer::framesReaded, this,&AudioContext::process_packet, Qt::QueuedConnection);
}

AudioContext::~AudioContext()
{
    delete audioSink;
    delete audio_outputer;
}

void AudioContext::mute_unmute()
{
    if(params->audio->isMuted)
        audioSink->setVolume(0);
    else
        audioSink->setVolume(last_volume);
}
void AudioContext::set_volume(){
    last_volume=params->audio->volume;
    if (!params->audio->isMuted)
        audioSink->setVolume(last_volume);
}

void AudioContext::pause_changed()
{
    if(params->isPaused)
        audioSink->suspend();
    else{
        audioSink->resume();
        emit audio_outputer->readyRead();
    }
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
    decode_packet(packet);
    auto queue = decoder.receive_frames();
    audio_outputer->frame_queue.push(std::move(queue));
    emit audio_outputer->framesPushed();
}

void AudioContext::decode_packet(Packet& packet)
{
    qreal packetTime = packet->pts * av_q2d(codec.timeBase);
    qreal currTime = sync->get_time() / 1000.0;
    qreal diff = currTime - packetTime;
    if (diff > 0.15){
        qDebug()<<"Packet is lating skipping:"<<diff<<"sec";
        return;
    }
    decoder.decode_packet(packet);
    emit audio_outputer->framesPushed();
}

qint64 AudioContext::buffer_available()
{
    qint64 available_bytes = maxBufferSize - audio_outputer->bytesAvailable();
    if (available_bytes <= 0 && !sync->isPaused)
        emit audio_outputer->readyRead();
    return available_bytes;
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


