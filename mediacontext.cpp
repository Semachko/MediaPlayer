#include <QtConcurrent>
#include <QElapsedTimer>
#include <QVideoFrame>
#include <utility>

#include "mediacontext.h"

MediaContext::MediaContext(){}

void MediaContext::setFile(const QUrl &filename)
{
    qDebug()<<filename.toLocalFile();
    if (format_context!=nullptr){
        delete video;
        delete audio;
        avformat_close_input(&format_context);
    }
    avformat_open_input(&format_context, filename.toLocalFile().toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);

    sync = new Synchronizer(this);

    video = new VideoContext(format_context, sync);
    videoThread = new QThread(this);
    video->moveToThread(videoThread);
    videoThread->start();

    audio = new AudioContext(format_context, sync);
    audioThread = new QThread(this);
    audio->moveToThread(audioThread);
    audioThread->start();


    fill_packetQueue();

    connect(video->output, &FrameOutput::imageToOutput, this, [this](QVideoFrame frame){
        //qDebug()<<"\033[32m[Screen]\033[0m Outputing image, size = "<<frame.size();
        videosink->setVideoFrame(frame);
        //fill_packetQueue();
    });
    connect(video,&VideoContext::requestPacket,this,[this](){
        fill_packetQueue();
    });
    connect(audio,&AudioContext::packetDecoded,this,[this](){
        fill_packetQueue();
    });

    // fill_videoPacketQueue();
    // fill_audioPacketQueue();

}

constexpr auto PACKET = "\033[33m[Packet]\033[0m";


void MediaContext::fill_packetQueue()
{
    //sync->check_pause();
    while(video->packetQueue.size() + audio->packetQueue.size() < QUEUE_MAX_SIZE)
        if (!push_packet_to_queues())
            return;
}

bool MediaContext::push_packet_to_queues()
{
    QMutexLocker _(&formatMutex);

    AVPacket *packet = av_packet_alloc();
    int res = av_read_frame(format_context, packet);
    if (res<0)
        return false;
    //qDebug()<<"Stream index"<<packet->stream_index;
    if (packet->stream_index == audio->stream_id){
        // audio->packetQueue.push(packet);
        // emit audio->newPacketReady();
    }
    else if(packet->stream_index == video->stream_id){
        video->packetQueue.push(packet);
        emit video->newPacketReady();
    }
    qDebug()<<PACKET<<"Video queue size ="<<video->packetQueue.size();
    qDebug()<<PACKET<<"Audio queue size ="<<audio->packetQueue.size();
    return true;
}

void MediaContext::playORpause()
{
    sync->play_or_pause();
    if(sync->isPaused)
        QMetaObject::invokeMethod(audio, [this](){
            audio->audioSink->suspend();
        });
    else
        QMetaObject::invokeMethod(audio, [this](){
            audio->audioSink->resume();
        });
}

void MediaContext::volumeChanged(qreal value)
{
    audio->last_volume=value;
    if (!audio->isMuted)
        audio->audioSink->setVolume(value);
}

void MediaContext::muteORunmute()
{
    if(audio->isMuted)
        audio->audioSink->setVolume(audio->last_volume);
    else
        audio->audioSink->setVolume(0);
    audio->isMuted=!audio->isMuted;
}


void MediaContext::timeChanged(qreal position)
{
    qDebug()<<"Position: "<<position;

    // int64_t seek_target = timestamp * AV_TIME_BASE; // timestamp в секундах
    // int res = av_seek_frame(format_context, -1, seek_target, AVSEEK_FLAG_BACKWARD);
    // if (res) {
    //     qWarning() << "Не удалось перемотать";
    //     isSeeking = false;
    //     return;
    // }

    // avcodec_flush_buffers(audio_codec_ctx);
    // avcodec_flush_buffers(video_codec_ctx);
}

QVideoSink *MediaContext::videoSink() const
{
    return videosink;
}

void MediaContext::setVideoSink(QVideoSink *sink)
{
    if (videosink == sink)
        return;
    videosink = sink;
    emit videoSinkChanged();
}





void MediaContext::fill_videoPacketQueue()
{
    // qDebug()<<PACKET<<"Filling video packet queue. Current size = "<<video->packetQueue.size();
    // while(video->packetQueue.size() < video->queueSize)
    //     if (!push_packet_to_queues())
    //         return;
}

void MediaContext::fill_audioPacketQueue()
{
    // qDebug()<<PACKET<<"Filling audio packet queue. Current size = "<<audio->packetQueue.size();
    // while(audio->packetQueue.size() < audio->queueSize)
    //     if (!push_packet_to_queues())
    //         return;
}
