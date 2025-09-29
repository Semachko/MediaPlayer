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
        QMetaObject::invokeMethod(params->videoSink, [this] {
            params->videoSink->setVideoFrame(QVideoFrame());
        }, Qt::QueuedConnection);
    }
    avformat_open_input(&format_context, params->file->path.toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);
    params->file->setGlobalTime(format_context->duration/1'000'000.0);
    clock = new Clock(params);

    initialize_demuxer();
    try_initialize_audio();
    try_initialize_video();
    try_initialize_subtitles();

    connect(&updateTimer, &QTimer::timeout, this, [this]() {
        qreal currtime = clock->get_time();
        params->setCurrentTime(currtime);
    });
    updateTimer.setInterval(100);
    QMetaObject::invokeMethod(&updateTimer, static_cast<void(QTimer::*)()>(&QTimer::start), Qt::QueuedConnection);
    connect(params,&MediaParameters::isPausedChanged,this,&Media::resume_pause_timer);
    connect(this,&Media::seekingPressed,this,&Media::seeking_pressed);
    connect(this,&Media::seekingReleased,this,&Media::seeking_released);
    connect(this,&Media::subtruct5sec,this,&Media::subtruct_5sec);
    connect(this,&Media::add5sec,this,&Media::add_5sec);
    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
    if(video && params->isPaused)
        video->outputer->process_one_image();
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
    qint64 seek_target = AV_TIME_BASE * (clock->get_time() + 5.0); // +5 sec
    if (seek_target > format_context->duration)
        seek_target = format_context->duration;
    seek_time(seek_target);
}

void Media::subtruct_5sec()
{
    qint64 seek_target = AV_TIME_BASE * (clock->get_time() - 5.0); // -5 sec
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
    qint64 seek_target = format_context->duration * position;
    seek_time(seek_target);
}

void Media::seeking_released()
{
    if (isSeekingPressed){
        params->setIsPaused(!params->isPaused);
        isSeekingPressed = false;
    }
}

void Media::seek_time(qint64 seek_target_us)
{
    qreal seek_target_s = seek_target_us /1'000'000.0;
    demuxer->mutex.lock();
    if(audio)
        audio->clear();
    if(video)
        video->clear();
    if(subs)
        subs->clear();
    clock->set_time(seek_target_s);
    params->setCurrentTime(seek_target_s);
    demuxer->seek(seek_target_us);
    QMetaObject::invokeMethod(demuxer,&Demuxer::demuxe_packets, Qt::QueuedConnection);
    demuxer->mutex.unlock();
    if(video && params->isPaused){
        qreal fps = av_q2d(video->codec.stream->avg_frame_rate);
        qreal fpus = 1'000'000.0 / fps;     // frames per microsecond
        qint64 total_dur = video->codec.stream->duration * av_q2d(video->codec.timeBase) * 1'000'000.0;
        qint64 time_until_end = total_dur - seek_target_us;
        qint64 diff = time_until_end - fpus * 1.5;
        if (diff>0)
            video->outputer->process_one_image();
    }
    is_seeking_processing = false;
}

void Media::initialize_demuxer()
{
    demuxer = new Demuxer(format_context);
    demuxerThread = new QThread();
    demuxer->moveToThread(demuxerThread);
    demuxerThread->start();
}
void Media::try_initialize_audio()
{
    int stream_id =  av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_id >= 0){
        audio = new AudioContext(format_context->streams[stream_id], clock, params, bufferization_time);
        audioThread = new QThread();
        audio->moveToThread(audioThread);
        audioThread->start();
        demuxer->add_context(audio);
    }
}
void Media::try_initialize_video()
{
    int stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (stream_id >= 0){
        video = new VideoContext(format_context->streams[stream_id], clock, params, bufferization_time);
        videoThread = new QThread();
        video->moveToThread(videoThread);
        videoThread->start();
        demuxer->add_context(video);

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
        qreal timeStep = 15'000.0 / format_context->duration;     // ~15ms step
        params->file->setTimeStep(timeStep);
    }
}
void Media::try_initialize_subtitles()
{
    std::vector<AVStream*> sub_streams;
    for (unsigned int i = 0; i < format_context->nb_streams; ++i) {
        AVStream *stream = format_context->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
            sub_streams.push_back(stream);
    }
    if (!sub_streams.empty()){
        subs = new Subtitles(std::move(sub_streams), clock, demuxer, params);
        subtitlesThread = new QThread();
        subs->moveToThread(subtitlesThread);
        subtitlesThread->start();
    }
}

void Media::delete_members()
{
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
    if (subs){
        subs->deleteLater();
        subtitlesThread->quit();
        subtitlesThread->wait();
        subtitlesThread->deleteLater();
        subs = nullptr;
    }

    avformat_close_input(&format_context);
    delete clock;
}
