#include <QtConcurrent>
#include <QElapsedTimer>
#include <QVideoFrame>
#include <utility>

#include "mediacontext.h"

MediaContext::MediaContext(){
    connect(this,&MediaContext::fileChanged,this,&MediaContext::set_file);
    connect(this,&MediaContext::playORpause,this,&MediaContext::resume_pause);
    connect(this,&MediaContext::volumeChanged,this,&MediaContext::change_volume);
    connect(this,&MediaContext::muteORunmute,this,&MediaContext::mute_unmute);
    connect(this,&MediaContext::timeChanged,this,&MediaContext::change_time);
}

void MediaContext::set_file(const QUrl &filename,QVideoSink* sink)
{
    videosink = sink;

    qDebug()<<filename.toLocalFile();
    if (format_context!=nullptr){
        delete video;
        delete audio;
        avformat_close_input(&format_context);
    }
    avformat_open_input(&format_context, filename.toLocalFile().toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);

    sync = new Synchronizer(this);

    video = new VideoContext(format_context, sync, videosink);
    videoThread = new QThread(this);
    video->moveToThread(videoThread);
    videoThread->start();

    audio = new AudioContext(format_context, sync);
    audioThread = new QThread(this);
    audio->moveToThread(audioThread);
    audioThread->start();

    demuxer = new Demuxer(format_context, sync, formatMutex,video,audio);
    demuxerThread = new QThread(this);
    demuxer->moveToThread(demuxerThread);
    demuxerThread->start();

    connect(video,&VideoContext::requestPacket,demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
    connect(audio,&AudioContext::requestPacket,demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);


    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
}

void MediaContext::resume_pause()
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

void MediaContext::change_volume(qreal value)
{
    audio->last_volume=value;
    if (!audio->isMuted)
        audio->audioSink->setVolume(value);
}

void MediaContext::mute_unmute()
{
    if(audio->isMuted)
        audio->audioSink->setVolume(audio->last_volume);
    else
        audio->audioSink->setVolume(0);
    audio->isMuted=!audio->isMuted;
}


void MediaContext::change_time(qreal position)
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



