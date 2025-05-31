#include <QtConcurrent>
#include <QElapsedTimer>
#include <QVideoFrame>
#include <utility>

#include "media.h"

Media::Media(){}
Media::~Media()


{ delete_members(); }

void Media::set_file(MediaParameters& parameters, QVideoSink* videosink)
{
    if (format_context != nullptr){
        delete_members();
        videosink->setVideoFrame(QVideoFrame());
    }

    avformat_open_input(&format_context, parameters.filepath.toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);

    emit outputGlobalTime(format_context->duration/1000);

    sync = new Synchronizer();

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, [this]() {
        if(!sync->isPaused){
            qint64 curr_time = sync->get_time();
            qreal pos = curr_time/(format_context->duration/1000.0);
            emit outputTime(curr_time, pos);
        }
    });
    updateTimer->start(100);

    // How many seconds forward we want to bufferize
    const qreal bufferization_time = 0.2;

    demuxer = new Demuxer(format_context, sync, formatMutex);
    demuxerThread = new QThread(this);
    demuxer->moveToThread(demuxerThread);
    demuxerThread->start();

    int stream_id = -1;
    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (stream_id >= 0){
        video = new VideoContext(videosink, format_context, sync, stream_id, bufferization_time);
        videoThread = new QThread(this);
        video->moveToThread(videoThread);
        videoThread->start();
        video->set_contrast(parameters.filters.contrast);
        video->set_brightness(parameters.filters.brightness);
        video->set_saturation(parameters.filters.saturation);

        demuxer->add_context(video->stream_id, video);
        connect(video,&VideoContext::requestPacket,demuxer,&Demuxer::demuxe_packets);
        connect(this,&Media::brightnessChanged,video,&VideoContext::set_brightness);
        connect(this,&Media::contrastChanged,video,&VideoContext::set_contrast);
        connect(this,&Media::saturationChanged,video,&VideoContext::set_saturation);
    }

    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_id >= 0){
        audio = new AudioContext(format_context, sync, stream_id, bufferization_time);
        audioThread = new QThread(this);
        audio->moveToThread(audioThread);
        audioThread->start();
        audio->set_low(parameters.equalizer.low);
        audio->set_mid(parameters.equalizer.mid);
        audio->set_high(parameters.equalizer.high);
        audio->set_speed(parameters.speed);
        audio->set_volume(parameters.volume);
        if(audio->isMuted != parameters.isMuted)
            mute_unmute();

        demuxer->add_context(audio->stream_id, audio);
        connect(audio,&AudioContext::requestPacket,demuxer,&Demuxer::demuxe_packets);
        connect(this,&Media::lowChanged,audio,&AudioContext::set_low);
        connect(this,&Media::midChanged,audio,&AudioContext::set_mid);
        connect(this,&Media::highChanged,audio,&AudioContext::set_high);
    }

    if(sync->isPaused != parameters.isPaused)
        resume_pause();
    isRepeating = parameters.isRepeating;
    change_speed(parameters.speed);

    connect(demuxer,&Demuxer::endReached,this,[this]() {
        change_time(0);
        if (!isRepeating)
            emit endReached();
    });

    if (video){
        lock_all_mutexes();
        output_one_image();
        unlock_all_mutexes();
    }


    connect(this,&Media::playORpause,this,&Media::resume_pause);
    connect(this,&Media::sliderPause,this,&Media::slider_pause);
    connect(this,&Media::volumeChanged,this,&Media::change_volume);
    connect(this,&Media::muteORunmute,this,&Media::mute_unmute);
    connect(this,&Media::timeChanged,this,&Media::change_time);
    connect(this,&Media::subtruct5sec,this,&Media::subtruct_5sec);
    connect(this,&Media::add5sec,this,&Media::add_5sec);
    connect(this,&Media::speedChanged,this,&Media::change_speed);
    connect(this,&Media::repeatingChanged,this,&Media::change_repeating);
    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
}

void Media::resume_pause()
{
    sync->play_or_pause();
    if(sync->isPaused)
        QMetaObject::invokeMethod(updateTimer, [this]() {
            updateTimer->stop();
        });
    else
        QMetaObject::invokeMethod(updateTimer, [this]() {
            updateTimer->start(100);
        });

    if (audio){
        if(sync->isPaused)
            QMetaObject::invokeMethod(updateTimer, [this]() {
                audio->audioSink->suspend();
            });
        else{
            QMetaObject::invokeMethod(updateTimer, [this]() {
                audio->audioSink->resume();
                emit audio->audioDevice->readyRead();
            });
        }
    }
}

void Media::slider_pause()
{
    if(!sync->isPaused){
        resume_pause();
        isSliderPause=true;
    }
}

void Media::change_repeating()
{
    isRepeating=!isRepeating;
}

void Media::change_volume(qreal value)
{
    if (audio)
        audio->set_volume(value);
}

void Media::mute_unmute()
{
    if(audio->isMuted)
        audio->audioSink->setVolume(audio->last_volume);
    else
        audio->audioSink->setVolume(0);
    audio->isMuted=!audio->isMuted;
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
    if(audio)
        audio->set_speed(speed);
    sync->clock->setSpeed(speed);
}

void Media::change_time(qreal position)
{
    int64_t seek_target = format_context->duration * position;
    seek_time(seek_target);
}

void Media::seek_time(int64_t seek_target)
{
    lock_all_mutexes();

    clear_all_buffers();
    av_seek_frame(format_context, -1, seek_target, AVSEEK_FLAG_BACKWARD);
    qint64 current_time = get_real_time_ms();
    sync->clock->set_time(current_time);

    if (video){
        emit video->requestPacket();
        output_one_image();
    }
    if (audio)
        emit audio->requestPacket();

    unlock_all_mutexes();

    if (isSliderPause) {
        resume_pause();
        isSliderPause=false;
    }
}


void Media::output_one_image()
{
    while(video->output->imageQueue.empty()){
        while(video->packetQueue.empty()){
            demuxer->push_packet_to_queues();
        }
        Packet packet;
        video->packetQueue.pop(packet);
        video->decode(packet);
        video->filter_and_output();
    }
    ImageFrame imageFrame;
    video->output->imageQueue.pop(imageFrame);
    video->videosink->setVideoFrame(imageFrame.image);
}


void Media::clear_all_buffers()
{
    if(audio){
        audio->packetQueue.clear();
        audio->audioDevice->clear();
        avcodec_flush_buffers(audio->codec_context);
    }
    if(video){
        video->packetQueue.clear();
        video->output->imageQueue.clear();
        avcodec_flush_buffers(video->codec_context);
    }
}
qint64 Media::get_real_time_ms()
{
    qint64 seeked_time_ms = -1;
    while (seeked_time_ms == -1){
        Packet temp_packet = make_shared_packet();
        av_read_frame(format_context, temp_packet.get());
        if (demuxer->medias.contains(temp_packet->stream_index)){
            seeked_time_ms = temp_packet->pts * av_q2d(format_context->streams[temp_packet->stream_index]->time_base) * 1000;
            IMediaContext* media = demuxer->medias[temp_packet->stream_index];
            media->packetQueue.push(std::move(temp_packet));
            emit media->newPacketArrived();
        }
    }
    return seeked_time_ms;
}

void Media::lock_all_mutexes()
{
    formatMutex.lock();
    if (audio)
        audio->audioMutex.lock();
    if (video){
        video->videoMutex.lock();
        video->output->queueMutex.lock();
    }
}

void Media::unlock_all_mutexes()
{
    formatMutex.unlock();
    if (audio)
        audio->audioMutex.unlock();
    if (video){
        video->videoMutex.unlock();
        video->output->queueMutex.unlock();
    }
}

void Media::delete_members()
{
    if(sync->isPaused)
        sync->play_or_pause();

    if (audio) {
        audioThread->quit();
        audioThread->wait();
        audioThread->deleteLater();
        audio->deleteLater();
        audio = nullptr;
    }
    if (video) {
        videoThread->quit();
        videoThread->wait();
        videoThread->deleteLater();
        video->output->abort = true;
        video->deleteLater();
        video = nullptr;
    }
    demuxerThread->quit();
    demuxerThread->wait();
    demuxerThread->deleteLater();
    demuxer->deleteLater();

    avformat_close_input(&format_context);
    sync->deleteLater();
    updateTimer->deleteLater();
}



