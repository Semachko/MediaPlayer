#include "media/media.h"
#include "media/codec.h"
#include "sync/scopedpause.h"

#include <QElapsedTimer>
#include <QVideoFrame>
#include <QtConcurrent>
#include <mutex>
#include <utility>

Media::Media(MediaParameters* parameters) :
        params(parameters),
        clock(parameters->clock) {
    connect(params->file, &FileParameters::pathChanged, this, &Media::set_file);
}
Media::~Media() { delete_members(); }

void Media::set_file() {
    if (format_context != nullptr) {
        delete_members();
        QMetaObject::invokeMethod(
            params->videoSink, [this] { params->videoSink->setVideoFrame(QVideoFrame()); }, Qt::QueuedConnection);
    }
    std::string filepath = params->file->path.toStdString();
    avformat_open_input(&format_context, params->file->path.toStdString().c_str(), nullptr, nullptr);
    avformat_find_stream_info(format_context, nullptr);
    params->file->setGlobalTime(format_context->duration / 1'000'000.0);

    initialize_demuxer();
    try_initialize_audio();
    try_initialize_video();
    try_initialize_subtitles();

    connect(params, &MediaParameters::seekingPressed, this, &Media::seeking_pressed, Qt::QueuedConnection);
    connect(params, &MediaParameters::seekingReleased, this, &Media::seeking_released, Qt::QueuedConnection);
    connect(params->clock, &Clock::timeChanged, this, &Media::seek_time, Qt::QueuedConnection);
    QMetaObject::invokeMethod(demuxer, &Demuxer::demuxe_packets, Qt::QueuedConnection);
    if (video && params->isPaused)
        video->outputer->process_one_image();
}

void Media::seeking_pressed(qreal time) {
    if (!params->isPaused) {
        ScopedPause pauser{params};
        clock->set_time(time);
    } else
        clock->set_time(time);
}

void Media::seeking_released(qreal time) {}

void Media::seek_time() {
    qreal seek_target_s = clock->get_time();
    qint64 seek_target_us = AV_TIME_BASE * seek_target_s;
    demuxer->mutex.lock();
    if (audio)
        audio->clear();
    if (video)
        video->clear();
    if (subs)
        subs->clear();
    demuxer->seek(seek_target_us);
    QMetaObject::invokeMethod(demuxer, &Demuxer::demuxe_packets, Qt::QueuedConnection);
    demuxer->mutex.unlock();
    if (video && params->isPaused) {
        qreal fps = av_q2d(video->codec.stream->avg_frame_rate);
        qreal fpus = 1'000'000.0 / fps;  // frames per microsecond
        qint64 total_dur = video->codec.stream->duration * av_q2d(video->codec.timeBase) * 1'000'000.0;
        qint64 time_until_end = total_dur - seek_target_us;
        qint64 diff = time_until_end - fpus * 1.5;
        if (diff > 0)
            video->outputer->process_one_image();
    }
    params->isSeeking = false;
}

void Media::initialize_demuxer() {
    demuxer = new Demuxer(format_context);
    demuxerThread = new QThread();
    demuxer->moveToThread(demuxerThread);
    demuxerThread->start();
}
void Media::try_initialize_audio() {
    int stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_id >= 0) {
        audio = new AudioContext(format_context->streams[stream_id], clock, params, BUFFERIZATION_TIME);
        audioThread = new QThread();
        audio->moveToThread(audioThread);
        audioThread->start();
        demuxer->add_context(audio);
    }
}
void Media::try_initialize_video() {
    int stream_id = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (stream_id >= 0) {
        video = new VideoContext(format_context->streams[stream_id], clock, params, BUFFERIZATION_TIME);
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
    } else {
        qreal timeStep = 15'000.0 / format_context->duration;  // ~15ms step
        params->file->setTimeStep(timeStep);
    }
}
void Media::try_initialize_subtitles() {
    subs = new Subtitles(params, clock);
    subtitlesThread = new QThread();
    subs->moveToThread(subtitlesThread);
    subtitlesThread->start();

    subs->add_subs(params->file->path);
}

void Media::delete_members() {
    disconnect(params, &MediaParameters::seekingPressed, this, &Media::seeking_pressed);
    disconnect(params, &MediaParameters::seekingReleased, this, &Media::seeking_released);
    disconnect(params->clock, &Clock::timeChanged, this, &Media::seek_time);

    demuxerThread->quit();
    demuxerThread->wait();
    delete demuxerThread;
    delete demuxer;

    if (audio) {
        audioThread->quit();
        audioThread->wait();
        delete audioThread;
        delete audio;
        audio = nullptr;
    }
    if (video) {
        videoThread->quit();
        videoThread->wait();
        delete videoThread;
        delete video;
        video = nullptr;

        previewThread->quit();
        previewThread->wait();
        delete previewThread;
        delete preview;
        preview = nullptr;
    }
    if (subs) {
        subtitlesThread->quit();
        subtitlesThread->wait();
        delete subtitlesThread;
        delete subs;
        subs = nullptr;
    }

    avformat_close_input(&format_context);
}
