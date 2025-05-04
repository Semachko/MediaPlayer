#include "audiocontext.h"
#include <qdebug.h>
#include <QMediaDevices>

AudioContext::AudioContext(AVFormatContext *format_context, Synchronizer* sync) : sync(sync)
{
    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_id<0)
        return;

    codec_parameters = format_context->streams[stream_id]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_parameters->codec_id);

    codec_context = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_context, codec_parameters);
    avcodec_open2(codec_context, codec, nullptr);

    format = QMediaDevices::defaultAudioOutput().preferredFormat();
    qDebug() << "Preferred format:";
    qDebug() << "Sample rate:" << format.sampleRate();
    qDebug() << "Channel count:" << format.channelCount();
    qDebug() << "Sample format:" << format.sampleFormat();
    if (!QMediaDevices::defaultAudioOutput().isFormatSupported(format)) {
        qWarning() << "Format not supported!";
    }
    else
        qDebug()<<"Format is supported!";

    AVChannelLayout outlayout;
    av_channel_layout_default(&outlayout, format.channelCount());
    AVChannelLayout inlayout;
    av_channel_layout_default(&inlayout, codec_context->ch_layout.nb_channels);

    resampleContext=swr_alloc();
    int ret = swr_alloc_set_opts2(
        &resampleContext,
        &outlayout,
        //AV_SAMPLE_FMT_FLT,
        convert_to_AVFormat(format.sampleFormat()),
        format.sampleRate(),
        //&inlayout,
        &codec_context->ch_layout,
        codec_context->sample_fmt,
        codec_context->sample_rate,
        0,
        nullptr
    );
    if (ret < 0) {
        qDebug() << "swr_alloc_set_opts2 failed:" << ret;
        return;
    }
    if ((ret = swr_init(resampleContext)) < 0) {
        qDebug() << "swr_init failed:" << ret;
        return;
    }

    audioDevice = new AudioIODevice(sync,this);
    audioSink = new QAudioSink(format, this);
    audioSink->setBufferSize(4096);
    audioSink->setVolume(last_volume);
    audioSink->start(audioDevice);
    audioSink->suspend();

    connect(this, &AudioContext::newPacketReady, this,&AudioContext::push_frame_to_buffer);
}

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto SAMPLE = "\033[35m[Sample]\033[0m";

void AudioContext::push_frame_to_buffer()
{
    sync->check_pause();
    //qDebug()<<DECODING<<"New packet arrived. Popping it";
    AVPacket* packet = packetQueue.pop();
    emit packetDecoded();
    qDebug()<<"Audio packet size = "<<packet->size;

    int ret = avcodec_send_packet(codec_context, packet);
    av_packet_free(&packet);
    if (ret < 0) {
        qDebug()<<"Error sending video packet: "<<ret;
        return;
    }

    AVFrame *frame = av_frame_alloc();
    while (avcodec_receive_frame(codec_context, frame) == 0)
    {
        //qDebug()<<DECODING<<"Sample decoded";
        //qDebug()<<DECODING<<"AV_SAMPLE_FMT_FLT = "<< AV_SAMPLE_FMT_FLT;
        //qDebug()<<DECODING<<"codec_context->sample_fmt = "<< codec_context->sample_fmt;
        int outputBufferSize = av_samples_get_buffer_size(
            nullptr,
            format.channelCount(),
            frame->nb_samples,
            convert_to_AVFormat(format.sampleFormat()),
            1
        );

        uint8_t *outputBuffer = (uint8_t*)av_malloc(outputBufferSize);
        if (!outputBuffer) {
            qWarning() << "Failed to allocate output buffer.";
            av_frame_unref(frame);
            continue;
        }
        int samplesConverted = swr_convert(
            resampleContext,
            &outputBuffer,
            //frame->nb_samples * format.sampleRate() / frame->sample_rate,
            frame->nb_samples,
            (const uint8_t **)frame->data,
            frame->nb_samples
            );
        if (samplesConverted < 0) {
            qWarning() << "Error converting audio.";
            av_free(outputBuffer);
            av_frame_unref(frame);
            continue;
        }

        int realSize = samplesConverted * format.channelCount() * format.bytesPerSample();

        //qDebug()<<SAMPLE<<"Outputbuffer size ="<<realSize;
        audioDevice->appendData(QByteArray((const char*)outputBuffer, realSize));
        av_free(outputBuffer);
        av_frame_unref(frame);
        // qDebug()<<"Audio state:"<<audioSink->state();
        // qDebug()<<"Audio error:"<<audioSink->error();
        // qDebug()<<"Audio volume:"<<audioSink->volume();
    }
    av_frame_free(&frame);
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
