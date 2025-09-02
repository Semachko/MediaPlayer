#include <QtConcurrent>
#include <QElapsedTimer>
#include <QVideoFrame>
#include <utility>
#include <mutex>

#include "media/media.h"
#include "media/codec.h"

Media::Media(MediaParameters* parameters)
    : params(parameters)
{
    connect(params->file,&FileParameters::pathChanged,this,&Media::set_file);
}
Media::~Media()
{ delete_members(); }

void Media::set_file()
{
    if (format_context != nullptr){
        delete_members();
        params->videoSink->setVideoFrame(QVideoFrame());
    }

    avformat_open_input(&format_context, params->file->path.toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);

    params->file->setGlobalTime(format_context->duration/1000);
    sync = new Synchronizer(params);

    connect(&updateTimer, &QTimer::timeout, this, [this]() {
        qint64 curr_time = sync->get_time();
        params->setCurrentTime(curr_time);
        // qreal pos = curr_time/(format_context->duration/1000.0);
        // emit outputTime(curr_time, pos);
    });
    updateTimer.start(100);

    // How many seconds forward we want to bufferize
    const qreal bufferization_time = 0.2;

    demuxer = new Demuxer(format_context, sync);
    demuxerThread = new QThread(this);
    demuxer->moveToThread(demuxerThread);
    demuxerThread->start();
    connect(demuxer,&Demuxer::endReached,this,[this]() {
//////////////////////////////////////////////////////
    });

    int stream_id = -1;
    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_id >= 0){
        audio = new AudioContext(format_context->streams[stream_id], sync, params, bufferization_time);
        audioThread = new QThread(this);
        audio->moveToThread(audioThread);
        audioThread->start();
        demuxer->add_context(audio);
        connect(audio,&AudioContext::requestPacket,demuxer,&Demuxer::demuxe_packets);
    }

    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (stream_id >= 0){
        video = new VideoContext(format_context->streams[stream_id], sync, params, bufferization_time);
        videoThread = new QThread(this);
        video->moveToThread(videoThread);
        videoThread->start();
        demuxer->add_context(video);
        connect(video,&VideoContext::requestPacket,demuxer,&Demuxer::demuxe_packets);

        qreal total_seconds = format_context->duration / 1'000'000.0;
        qreal fps = av_q2d(video->codec.stream->avg_frame_rate);
        qint64 total_frames = total_seconds * fps;
        qreal timeStep = 1.0 / total_frames;
        params->file->setTimeStep(timeStep);
    }else{
        qreal timeStep = format_context->duration / 15'000;     // ~15ms step
        params->file->setTimeStep(timeStep);
    }
    connect(params,&MediaParameters::isPausedChanged,this,&Media::resume_pause);
    connect(this,&Media::seekingPressed,this,&Media::seeking_pressed);
    connect(this,&Media::seekingReleased,this,&Media::seeking_released);
    connect(this,&Media::subtruct5sec,this,&Media::subtruct_5sec);
    connect(this,&Media::add5sec,this,&Media::add_5sec);
    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
    if(video){
        if(sync->isPaused)
            video->output->process_one_image();
        else{
            sync->play_or_pause();
            video->output->process_one_image();
            sync->play_or_pause();
        }
    }
}

void Media::resume_pause()
{
    QMetaObject::invokeMethod(&updateTimer, [this]() {
        if(params->isPaused)
            updateTimer.stop();
        else
            updateTimer.start(100);
    });
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

void Media::seeking_pressed(qreal position)
{
    if (!isSeeking && !params->isPaused){
        params->setIsPaused(!params->isPaused);
        isSeeking = true;
    }
    int64_t seek_target = format_context->duration * position;
    seek_time(seek_target);
}

void Media::seeking_released()
{
    if (isSeeking){
        params->setIsPaused(!params->isPaused);
        isSeeking = false;
    }
}

void Media::seek_time(int64_t seek_target)
{
    demuxer->mutex.lock();
    if(audio){
        std::lock_guard _(audio->mutex);
        audio->packet_queue.clear();
        audio->audio_outputer->clear();
        audio->decoder.clear_decoder();
    }
    if(video){
        std::lock_guard _(video->mutex);
        video->packet_queue.clear();
        video->output->image_queue.clear();
        video->decoder.clear_decoder();
    }
    av_seek_frame(format_context, -1, seek_target, AVSEEK_FLAG_BACKWARD);
    qint64 current_time = get_real_time_ms();
    sync->clock->set_time(current_time);
    demuxer->mutex.unlock();

    if (video)
        video->output->process_one_image();
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
            media->packet_queue.push(std::move(temp_packet));
            emit media->newPacketArrived();

        }
    }
    return seeked_time_ms;
}

void Media::delete_members()
{
    if(!sync->isPaused)
        sync->play_or_pause();
    disconnect(this,&Media::playORpause,this,&Media::resume_pause);
    disconnect(this,&Media::seekingPressed,this,&Media::seeking_pressed);
    disconnect(this,&Media::subtruct5sec,this,&Media::subtruct_5sec);
    disconnect(this,&Media::add5sec,this,&Media::add_5sec);

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
}
