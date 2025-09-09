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
    });
    updateTimer.setInterval(100);
    QMetaObject::invokeMethod(&updateTimer, static_cast<void(QTimer::*)()>(&QTimer::start), Qt::QueuedConnection);

    // How many seconds forward we want to bufferize
    const qreal bufferization_time = 0.2;

    demuxer = new Demuxer(format_context, sync);
    demuxerThread = new QThread();
    demuxer->moveToThread(demuxerThread);
    demuxerThread->start();

    int stream_id = -1;
    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_id >= 0){
        audio = new AudioContext(format_context->streams[stream_id], sync, params, bufferization_time);
        audioThread = new QThread();
        audio->moveToThread(audioThread);
        audioThread->start();
        demuxer->add_context(audio);
        connect(audio,&AudioContext::requestPacket,demuxer,&Demuxer::demuxe_packets);
    }

    stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (stream_id >= 0){
        video = new VideoContext(format_context->streams[stream_id], sync, params, bufferization_time);
        videoThread = new QThread();
        video->moveToThread(videoThread);
        videoThread->start();
        demuxer->add_context(video);
        connect(video,&VideoContext::requestPacket,demuxer,&Demuxer::demuxe_packets);

        qreal total_seconds = format_context->duration / 1'000'000.0;
        qreal fps = av_q2d(video->codec.stream->avg_frame_rate);
        qint64 total_frames = total_seconds * fps;
        qreal timeStep = 1.0 / total_frames;
        params->file->setTimeStep(timeStep);

        preview = new VideoPreview(params);
        previewThread = new QThread();
        preview->moveToThread(previewThread);
        previewThread->start();
    }else{
        qreal timeStep = format_context->duration / 15'000;     // ~15ms step
        params->file->setTimeStep(timeStep);
    }
    connect(params,&MediaParameters::isPausedChanged,this,&Media::resume_pause_timer);
    connect(this,&Media::seekingPressed,this,&Media::seeking_pressed);
    connect(this,&Media::seekingReleased,this,&Media::seeking_released);
    connect(this,&Media::subtruct5sec,this,&Media::subtruct_5sec);
    connect(this,&Media::add5sec,this,&Media::add_5sec);
    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
    if(video && params->isPaused)
        video->output->process_one_image();
}

void Media::resume_pause_timer()
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
    if (!isSeekingPressed && !params->isPaused){
        params->setIsPaused(!params->isPaused);
        isSeekingPressed = true;
    }
    int64_t seek_target = format_context->duration * position;
    seek_time(seek_target);
    is_seeking_processing = false;
}

void Media::seeking_released()
{
    if (isSeekingPressed){
        params->setIsPaused(!params->isPaused);
        isSeekingPressed = false;
    }
}

void Media::seek_time(int64_t seek_target)
{
    demuxer->mutex.lock();
    if(audio)
        audio->clear();
    if(video)
        video->clear();
    sync->set_time(seek_target/1000);
    params->setCurrentTime(seek_target/1000);
    demuxer->seek(seek_target);
    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
    demuxer->mutex.unlock();

    if(video){
        qreal fps = av_q2d(video->codec.stream->avg_frame_rate);
        qreal fpus = 1'000'000.0 / fps;                                         // frames per microsecond
        qint64 total_dur = video->codec.stream->duration * av_q2d(video->codec.timeBase) * 1'000'000.0;
        qint64 time_until_end = total_dur - seek_target;
        qint64 diff = time_until_end - fpus * 1.5;
        if (diff>0)
            video->output->process_one_image();
    }
}

void Media::delete_members()
{
    bool was_paused = params->isPaused;
    if(was_paused)
        params->setIsPaused(false);

    disconnect(this,&Media::playORpause,this,&Media::resume_pause_timer);
    disconnect(this,&Media::seekingPressed,this,&Media::seeking_pressed);
    disconnect(this,&Media::subtruct5sec,this,&Media::subtruct_5sec);
    disconnect(this,&Media::add5sec,this,&Media::add_5sec);

    demuxer->deleteLater();
    demuxerThread->quit();
    demuxerThread->wait();
    demuxerThread->deleteLater();

    if (audio) {
        audio->deleteLater();
        audioThread->quit();
        audioThread->wait();
        audioThread->deleteLater();
        audio = nullptr;
    }
    if (video) {
        video->deleteLater();
        videoThread->quit();
        videoThread->wait();
        videoThread->deleteLater();
        video = nullptr;

        preview->deleteLater();
        previewThread->quit();
        previewThread->wait();
        previewThread->deleteLater();
        preview = nullptr;
    }

    avformat_close_input(&format_context);
    delete sync;

    if(was_paused)
        params->setIsPaused(true);
}
