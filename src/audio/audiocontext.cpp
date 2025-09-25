#include <QDebug>
#include <QMediaDevices>

#include "audio/audiocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto SAMPLE = "\033[35m[Sample]\033[0m";

AudioContext::AudioContext(AVStream* stream,  Clock* clock_, MediaParameters* params_, qreal bufferization_time_)
    :
    IMediaContext(stream,10),
    bufferization_time(bufferization_time_),
    last_volume(params->audio->volume),
    //equalizer(codec, params),
    params(params_),
    clock(clock_)
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
    outputFormat = SampleFormat{convert_to_AVFormat(format.sampleFormat()),format.sampleRate(),format.bytesPerSample(),outlayout};

    qDebug()<<"Audio sample rate:"<< codec.parameters->sample_rate;
    qDebug()<<"Audio sample format:"<<av_get_sample_fmt_name(static_cast<AVSampleFormat>(codec.parameters->format));
    qDebug()<<"Audio channels:"<< codec.parameters->ch_layout.nb_channels;
    qDebug()<<"Audio bitrate:"<<codec.parameters->bit_rate;

    // Getting MAX SIZE of audio output buffer
    bytes_per_sec = outputFormat.layout.nb_channels * outputFormat.sample_rate * av_get_bytes_per_sample((AVSampleFormat)outputFormat.format);
    MAX_BUFFER_SIZE = bytes_per_sec * bufferization_time;

    audioSink = new QAudioSink(format, this);
    outputer = new AudioOutputer(clock, codec, outputFormat, params, audioSink);
    audioSink->setVolume(last_volume);
    audioSink->setBufferSize(8192);
    if(params->audio->isMuted)
        mute_unmute();
    outputThread = new QThread();
    outputer->moveToThread(outputThread);
    outputThread->start();

    connect(params, &MediaParameters::isPausedChanged, this,&AudioContext::pause_changed);
    connect(params->audio, &AudioParameters::isMutedChanged,this,&AudioContext::mute_unmute);
    connect(params->audio, &AudioParameters::volumeChanged,this,&AudioContext::set_volume);

    connect(this, &AudioContext::newPacketArrived, this,&AudioContext::process_packet);
    connect(outputer, &AudioOutputer::requestFrame, this,&AudioContext::process_packet);
    connect(this,&IMediaContext::endReached, [this]{decoder.drain_decoder();});

    audioSink->start(outputer);
    if(params->isPaused)
        audioSink->suspend();
}

AudioContext::~AudioContext()
{
    outputer->deleteLater();
    outputThread->quit();
    outputThread->wait();
    outputThread->deleteLater();
}

void AudioContext::process_packet()
{
    std::lock_guard _(mutex);
    if (buffer_available() <= 0)
        return;
    Packet packet = packet_queue.try_pop();
    if (!decoder.is_drained())
        emit requestPacket();
    if (packet)
        decode_packet(packet);
    get_and_output_samples();
}

void AudioContext::decode_packet(Packet& packet)
{
    decoder.decode_packet(packet);
}

void AudioContext::get_and_output_samples()
{
    auto queue = decoder.receive_frames();
    while(!queue.empty())
    {
        Frame frame = queue.dequeue();
        qreal frametime = frame->best_effort_timestamp * av_q2d(codec.timeBase);
        qreal currtime = clock->get_time();
        qreal delay = frametime - currtime;
        if (delay < 0)
            continue;
        outputer->push_frame(frame);
    }
}

qint64 AudioContext::buffer_available()
{
    qint64 available_bytes = MAX_BUFFER_SIZE - outputer->bytesAvailable();
    return available_bytes;
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
        emit outputer->readyRead();
    }
}

void AudioContext::clear()
{
    std::lock_guard _(mutex);
    packet_queue.clear();
    decoder.clear_decoder();
    outputer->clear();
    outputer->reset();
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


