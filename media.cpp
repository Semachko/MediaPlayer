#include <QtConcurrent>
#include <QElapsedTimer>
#include <QVideoFrame>
#include <utility>

#include "media.h"

Media::Media(){
    connect(this,&Media::fileChanged,this,&Media::set_file);
    connect(this,&Media::playORpause,this,&Media::resume_pause);
    connect(this,&Media::sliderPause,this,&Media::slider_pause);
    connect(this,&Media::volumeChanged,this,&Media::change_volume);
    connect(this,&Media::muteORunmute,this,&Media::mute_unmute);
    connect(this,&Media::timeChanged,this,&Media::change_time);
    connect(this,&Media::subtruct5sec,this,&Media::subtruct_5sec);
    connect(this,&Media::add5sec,this,&Media::add_5sec);
    connect(this,&Media::speedChanged,this,&Media::change_speed);
}
Media::~Media()
{ delete_members(); }

void Media::set_file(const QUrl &filename,QVideoSink* sink,bool isPlaying)
{
    if (format_context != nullptr)
        delete_members();

    videosink = sink;
    qDebug()<<filename.toLocalFile();

    avformat_open_input(&format_context, filename.toLocalFile().toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);

    emit outputGlobalTime(format_context->duration/1000);

    sync = new Synchronizer();


    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, [this]() {
        qint64 curr_time = sync->get_time();
        qreal pos = curr_time/(format_context->duration/1000.0);
        emit outputTime(curr_time, pos);
    });
    updateTimer->start(100);

    // How many seconds forward we want to bufferize
    const qreal bufferization_time = 0.2;

    demuxer = new Demuxer(format_context, sync, formatMutex);
    demuxerThread = new QThread(this);
    demuxer->moveToThread(demuxerThread);
    demuxerThread->start();

    video = new VideoContext(videosink, format_context, sync, bufferization_time);
    if (video->stream_id >= 0){
        videoThread = new QThread(this);
        video->moveToThread(videoThread);
        videoThread->start();
        demuxer->add_context(video->stream_id, video);
    }

    audio = new AudioContext(format_context, sync, bufferization_time);
    if (audio->stream_id >= 0){
        audioThread = new QThread(this);
        audio->moveToThread(audioThread);
        audioThread->start();
        demuxer->add_context(audio->stream_id, audio);
    }


    connect(video,&VideoContext::requestPacket,demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
    connect(audio,&AudioContext::requestPacket,demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);

    connect(this,&Media::brightnessChanged,video,&VideoContext::set_brightness);
    connect(this,&Media::contrastChanged,video,&VideoContext::set_contrast);
    connect(this,&Media::saturationChanged,video,&VideoContext::set_saturation);

    connect(this,&Media::lowChanged,audio,&AudioContext::set_low);
    connect(this,&Media::midChanged,audio,&AudioContext::set_mid);
    connect(this,&Media::highChanged,audio,&AudioContext::set_high);

    if(sync->isPaused != !isPlaying)
        sync->play_or_pause();

    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
}

void Media::resume_pause()
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

void Media::slider_pause()
{
    if(!sync->isPaused){
        resume_pause();
        isTemporaryPaused=true;
    }
}

void Media::change_volume(qreal value)
{
    audio->last_volume=value;
    if (!audio->isMuted)
        audio->audioSink->setVolume(value);
}

void Media::mute_unmute()
{
    if(audio->isMuted)
        audio->audioSink->setVolume(audio->last_volume);
    else
        audio->audioSink->setVolume(0);
    audio->isMuted=!audio->isMuted;
}


void Media::seek_time(int64_t seek_target)
{
    QMutexLocker f(&formatMutex);
    QMutexLocker v(&video->decodingMutex);
    QMutexLocker a(&audio->audioMutex);

    audio->audioSink->stop();
    QAudioSink* oldSink = audio->audioSink;
    oldSink->deleteLater();
    audio->audioSink = new QAudioSink(audio->format, this);
    audio->audioSink->setVolume(audio->last_volume);
    audio->audioSink->start(audio->audioDevice);
    audio->audioSink->suspend();

    video->packetQueue.clear();
    audio->packetQueue.clear();

    avcodec_flush_buffers(video->codec_context);
    avcodec_flush_buffers(audio->codec_context);

    QMutexLocker o(&video->output->queueMutex);
    video->output->imageQueue.clear();
    //audio->audioDevice->clear();
    audio->audioDevice->readAll();

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

void Media::change_time(qreal position)
{
    int64_t seek_target = format_context->duration * position;
    seek_time(seek_target);
}

void Media::add_5sec()
{
    int64_t sec5 = AV_TIME_BASE * 5;
    int64_t seek_target = (sync->get_time()*1000) + sec5;
    if (seek_target > format_context->duration)
        seek_target = format_context->duration;
    seek_time(seek_target);
}

void Media::subtruct_5sec()
{
    int64_t sec5 = AV_TIME_BASE * 5;
    int64_t seek_target = (sync->get_time()*1000) - sec5;
    if (seek_target < 0)
        seek_target = 0;
    seek_time(seek_target);
}

void Media::change_speed(qreal speed)
{
    sync->clock->setSpeed(speed);
    audio->set_speed(speed);
}

void Media::delete_members()
{
    if(sync->isPaused)
        sync->play_or_pause();

    demuxerThread->quit();
    audioThread->quit();
    videoThread->quit();

    demuxerThread->wait();
    audioThread->wait();
    videoThread->wait();

    audioThread->deleteLater();
    videoThread->deleteLater();
    demuxerThread->deleteLater();

    audio->deleteLater();
    video->deleteLater();
    demuxer->deleteLater();

    avformat_close_input(&format_context);
    sync->deleteLater();
    updateTimer->deleteLater();
}



