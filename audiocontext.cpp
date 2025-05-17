#include <QDebug>
#include <QMediaDevices>

#include "audiocontext.h"
#include "frame.h"

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto SAMPLE = "\033[35m[Sample]\033[0m";

AudioContext::AudioContext(AVFormatContext *format_context, Synchronizer* sync, qreal bufferization_time)
    :
    IMediaContext(20),
    sync(sync)
{
    // Finding audio STREAM
    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_id<0)
        return;

    timeBase = format_context->streams[stream_id]->time_base;

    // Initiating CODEC
    AVCodecParameters* codec_parameters = format_context->streams[stream_id]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_parameters->codec_id);
    codec_context = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_context, codec_parameters);
    avcodec_open2(codec_context, codec, nullptr);

    // Initiating output audio device FORMAT
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

    // Initiating output audio CONVERTER
    AVChannelLayout outlayout;
    av_channel_layout_default(&outlayout, format.channelCount());
    SampleFormat outputFormat{convert_to_AVFormat(format.sampleFormat()),format.sampleRate(),outlayout};
    converter = new SampleConverter(codec_context,outputFormat);

    // Getting MAX SIZE of audio output buffer
    qint64 bytesPerSample = av_get_bytes_per_sample((AVSampleFormat)outputFormat.format);
    qint64 bytesPerSecond = bytesPerSample * outputFormat.layout.nb_channels * outputFormat.sample_rate;
    maxBufferSize = bytesPerSecond * bufferization_time;


    // Initiating audio output DEVICE
    audioDevice = new AudioIODevice(sync,this);
    audioSink = new QAudioSink(format, this);
    audioSink->setVolume(last_volume);
    audioSink->start(audioDevice);
    audioSink->suspend();

    connect(this, &AudioContext::newPacketArrived, this,&AudioContext::decode_and_output, Qt::QueuedConnection);
    connect(audioDevice, &AudioIODevice::dataReaded, this,&AudioContext::decode_and_output, Qt::QueuedConnection);
}

AudioContext::~AudioContext()
{
    delete converter;
    delete equalizer;
    delete audioSink;
    delete audioDevice;
    delete codec_context;
}

void AudioContext::decode_and_output()
{
    sync->check_pause();
    QMutexLocker _(&audioMutex);

    if (buffer_available() == 0)
        return;

    if (packetQueue.size() == 0){
        emit requestPacket();
        return;
    }

    Packet packet = packetQueue.pop();
    emit requestPacket();

    int result = avcodec_send_packet(codec_context, packet.get());;
    if (result < 0) {
        qDebug()<<"Error decoding audio packet: "<<result;
        return;
    }

    equalizer_and_output();
}

void AudioContext::equalizer_and_output()
{
    Frame frame = make_shared_frame();
    while (avcodec_receive_frame(codec_context, frame.get()) == 0)
    {
        Frame equalized_frame = equalizer->applyEqualizer(frame);
        if (!equalized_frame)
            continue;
        Frame converted_frame = converter->convert(equalized_frame);
        if (!converted_frame)
            continue;
        int size = converted_frame->nb_samples * converted_frame->ch_layout.nb_channels * format.bytesPerSample();
        audioDevice->appendData(QByteArray((const char*)converted_frame->data[0], size));
        av_frame_unref(frame.get());
    }
}

qint64 AudioContext::buffer_available()
{
    qint64 available_bytes = maxBufferSize - audioDevice->bytesAvailable();
    if (available_bytes <= 0)
        return 0;
    return available_bytes;
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
void AudioContext::set_speed(qreal speed)
{
    equalizer->set_speed(speed);
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


