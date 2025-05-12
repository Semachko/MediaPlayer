#include <QtConcurrent>
#include <QElapsedTimer>
#include <QVideoFrame>
#include <utility>

#include "mediacontext.h"

MediaContext::MediaContext(){
    connect(this,&MediaContext::fileChanged,this,&MediaContext::set_file);
    connect(this,&MediaContext::playORpause,this,&MediaContext::resume_pause);
    connect(this,&MediaContext::sliderPause,this,&MediaContext::slider_pause);
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

    emit outputGlobalTime(format_context->duration/1000);

    sync = new Synchronizer(this);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, [this]() {
        qint64 curr_time = sync->get_time();
        qreal pos = curr_time/(format_context->duration/1000.0);
        emit outputTime(curr_time, pos);
    });
    updateTimer->start(100);

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

    connect(this,&MediaContext::brightnessChanged,video,&VideoContext::set_brightness);
    connect(this,&MediaContext::contrastChanged,video,&VideoContext::set_contrast);
    connect(this,&MediaContext::saturationChanged,video,&VideoContext::set_saturation);

    connect(this,&MediaContext::lowChanged,audio,&AudioContext::set_low);
    connect(this,&MediaContext::midChanged,audio,&AudioContext::set_mid);
    connect(this,&MediaContext::highChanged,audio,&AudioContext::set_high);

    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
}

void MediaContext::resume_pause()
{
    sync->play_or_pause();
    if(sync->isPaused){
        updateTimer->stop();
        QMetaObject::invokeMethod(audio, [this](){
            audio->audioSink->suspend();
        });
    }
    else{
        updateTimer->start(100);
        QMetaObject::invokeMethod(audio, [this](){
            audio->audioSink->resume();
        });
    }
}

void MediaContext::slider_pause()
{
    if(!sync->isPaused){
        resume_pause();
        isTemporaryPaused=true;
    }
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
    QMutexLocker f(&formatMutex);
    QMutexLocker v(&video->decodingMutex);
    QMutexLocker a(&audio->decodingMutex);
    video->packetQueue.clear();
    audio->packetQueue.clear();

    avcodec_flush_buffers(video->codec_context);
    avcodec_flush_buffers(audio->codec_context);

    QMutexLocker o(&video->output->queueMutex);
    video->output->imageQueue.clear();
    //audio->audioDevice->clear();
    audio->audioDevice->readAll();

    audio->audioSink->stop();
    QAudioSink* oldSink = audio->audioSink;
    oldSink->deleteLater();
    audio->audioSink = new QAudioSink(audio->format, this);
    audio->audioSink->setVolume(audio->last_volume);
    audio->audioSink->start(audio->audioDevice);
    audio->audioSink->suspend();

    int64_t seek_target = format_context->duration * position;
    sync->clock->set_time(seek_target/1000);

    int res = av_seek_frame(format_context, -1, seek_target, AVSEEK_FLAG_BACKWARD);
    if (res) {
        qWarning() << "No such time";
        return;
    }
    if(isTemporaryPaused){
        resume_pause();
        isTemporaryPaused=false;
    }
}



