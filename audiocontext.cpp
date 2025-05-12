#include <QDebug>
#include <QMediaDevices>

#include "audiocontext.h"
#include "frame.h"

AudioContext::AudioContext(AVFormatContext *format_context, Synchronizer* sync) : sync(sync), packetQueue(16)
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
        qWarning() << "Format not supported!";
        stream_id = -1;
        return;
    } else
        qDebug()<<"Format is supported!";

    equalizer = new Equalizer(codec_context);

    AVChannelLayout outlayout;
    av_channel_layout_default(&outlayout, format.channelCount());

    resampleContext=swr_alloc();
    int ret = swr_alloc_set_opts2(
        &resampleContext,
        &outlayout,
        convert_to_AVFormat(format.sampleFormat()),
        format.sampleRate(),
        &codec_context->ch_layout,
        AV_SAMPLE_FMT_FLTP,
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
    audioSink->setVolume(last_volume);
    audioSink->start(audioDevice);
    audioSink->suspend();
    // qDebug()<<"AudioSink bufferSize: "<<audioSink->bufferSize();
    // qDebug()<<"AudioSink bytesFree: "<<audioSink->bytesFree();

    connect(this, &AudioContext::newPacketArrived, this,&AudioContext::push_frame_to_buffer, Qt::QueuedConnection);
    connect(audioDevice, &AudioIODevice::dataReaded, this,&AudioContext::push_frame_to_buffer, Qt::QueuedConnection);
}

constexpr auto DECODING = "\033[31m[Decoding]\033[0m";
constexpr auto SAMPLE = "\033[35m[Sample]\033[0m";

void AudioContext::push_frame_to_buffer()
{
    sync->check_pause();
    QMutexLocker _(&decodingMutex);
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
        qDebug()<<"Error sending video packet: "<<ret;
        return;
    }

    Frame frame;
    while (avcodec_receive_frame(codec_context, frame.get()) == 0)
    {
        // AVFrame* out_frame = av_frame_alloc();
        // out_frame->ch_layout = frame->ch_layout;
        // out_frame->sample_rate = frame->sample_rate;
        // out_frame->format = AV_SAMPLE_FMT_FLTP;

        // SwrContext* toFLTP = swr_alloc();
        // int ret = swr_alloc_set_opts2(
        //     &toFLTP,
        //     &out_frame->ch_layout,
        //     static_cast<AVSampleFormat>(out_frame->format),
        //     out_frame->sample_rate,
        //     &frame->ch_layout,
        //     static_cast<AVSampleFormat>(frame->format),
        //     frame->sample_rate,
        //     0,
        //     nullptr
        //     );
        // if (ret < 0) {
        //     qDebug() << "swr_alloc_set_opts2 failed:" << ret;
        //     return;
        // }
        // if ((ret = swr_init(toFLTP)) < 0) {
        //     qDebug() << "swr_init failed:" << ret;
        //     return;
        // }
        // // out_frame->nb_samples = av_rescale_rnd(
        // //     swr_get_delay(toFLTP, frame->sample_rate) + frame->nb_samples,
        // //     out_frame->sample_rate,
        // //     frame->sample_rate,
        // //     AV_ROUND_UP
        // // );
        // out_frame->nb_samples = frame->nb_samples;
        // if ((ret = av_frame_get_buffer(out_frame, 0)) < 0) {
        //     qDebug() << "av_frame_get_buffer failed:" << ret;
        //     return;
        // }
        // ret = swr_convert(
        //     toFLTP,
        //     out_frame->data,
        //     out_frame->nb_samples,
        //     (const uint8_t**)frame->data,
        //     frame->nb_samples
        // );
        // //qDebug()<<"Number of converted samples ="<<ret;

        // av_frame_unref(frame.get());

        // av_frame_free(&out_frame);


        // int outputBufferSize = av_samples_get_buffer_size(
        //     nullptr,
        //     format.channelCount(),
        //     filtered_frame->nb_samples,
        //     convert_to_AVFormat(format.sampleFormat()),
        //     1
        // );
        // uint8_t *outputBuffer = (uint8_t*)av_malloc(outputBufferSize);
        // if (!outputBuffer) {
        //     qWarning() << "Failed to allocate output buffer.";
        //     av_frame_unref(filtered_frame.get());
        //     continue;
        // }
        // int samplesConverted = swr_convert(
        //     resampleContext,
        //     &outputBuffer,
        //     filtered_frame->nb_samples,
        //     (const uint8_t **)filtered_frame->data,
        //     filtered_frame->nb_samples
        // );
        // if (samplesConverted < 0) {
        //     qWarning() << "Error converting audio.";
        //     av_free(outputBuffer);
        //     av_frame_unref(filtered_frame.get());
        //     continue;
        // }

        // int realSize = samplesConverted * format.channelCount() * format.bytesPerSample();

        //qDebug()<<SAMPLE<<"Outputbuffer size ="<<realSize;

        Frame filtered_frame = equalizer->applyEqualizer(frame.get());

        SampleConverter converter;
        converter.set_out_sample_rate(format.sampleRate());
        converter.set_out_format(convert_to_AVFormat(format.sampleFormat()));
        AVChannelLayout outlayout;
        av_channel_layout_default(&outlayout, format.channelCount());
        converter.set_out_layout(outlayout);

        AVFrame* result = converter.convert(filtered_frame.get());

        int size = result->nb_samples * result->ch_layout.nb_channels * format.bytesPerSample();
        audioDevice->appendData(QByteArray((const char*)result->data[0], size));
        av_frame_free(&result);
        av_frame_free(&filtered_frame.frame);
        av_frame_unref(frame.get());
        // qDebug()<<"Audio state:"<<audioSink->state();
        // qDebug()<<"Audio error:"<<audioSink->error();
        // qDebug()<<"Audio volume:"<<audioSink->volume();
    }
    av_frame_free(&frame.frame);
}

void AudioContext::set_low(qreal value)
{
    qDebug("Invoking set_low");
    equalizer->set_low(value);
}

void AudioContext::set_mid(qreal value)
{
    qDebug("Invoking set_mid");
    equalizer->set_mid(value);
}

void AudioContext::set_high(qreal value)
{
    qDebug("Invoking set_high");
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
