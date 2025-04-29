#include <QtConcurrent>
#include <QElapsedTimer>
#include <QVideoFrame>
#include <utility>

#include "mediacontext.h"
#include "libavutil/frame.h"

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

    video = new VideoContext(format_context, sync, videosink);
    videoThread = new QThread(this);
    video->moveToThread(videoThread);
    videoThread->start();

    audio = new AudioContext(format_context);
    audioThread = new QThread(this);
    audio->moveToThread(audioThread);
    audioThread->start();


    connect(video, &VideoContext::frameReady, this, [this](QVideoFrame frame){
        videosink->setVideoFrame(frame);
    });

    auto _ = QtConcurrent::run([this](){
        processMedia();
    });
}

void MediaContext::playORpause()
{
    sync->playORpause();
    if(sync->isPaused)
        QMetaObject::invokeMethod(audio, [this](){
            audio->audioSink->suspend();
        });
    else
        QMetaObject::invokeMethod(audio, [this](){
            audio->audioSink->resume();
        });
}

void MediaContext::processMedia()
{
    AVPacket *packet = av_packet_alloc();
    while (av_read_frame(format_context, packet) >= 0)
    {
        QMutexLocker locker(&sync->playORpause_mutex);
        while(sync->isPaused)
            sync->pauseWait.wait(&sync->playORpause_mutex);

        if (packet->stream_index == audio->stream_id)
        {
            AVPacket* packet_copy = av_packet_alloc();
            av_packet_ref(packet_copy, packet);
            QMetaObject::invokeMethod(audio, [this, packet_copy](){
                audio->process(packet_copy);
            });
        }
        else if(packet->stream_index == video->stream_id){
            AVPacket* packet_copy = av_packet_alloc();
            av_packet_ref(packet_copy, packet);
            QMetaObject::invokeMethod(video, [this, packet_copy](){
                video->process(packet_copy);
            });
        }
        av_packet_unref(packet);
    }
    av_packet_free(&packet);
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

// void MediaContext::processMedia()
// {
//     SwsContext* sws = sws_getContext(
//         video->codec_context->width, video->codec_context->height, video->codec_context->pix_fmt,
//         video->codec_context->width, video->codec_context->height, AV_PIX_FMT_RGB32,
//         SWS_BICUBIC, nullptr, nullptr, nullptr
//         );

//     AVPacket* packet = av_packet_alloc();
//     AVFrame* frame = av_frame_alloc();
//     AVFrame* rgbFrame = av_frame_alloc();

//     std::vector<uint8_t> buffer(av_image_get_buffer_size(AV_PIX_FMT_RGB32, video->codec_context->width, video->codec_context->height, 1));
//     av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer.data(),
//                          AV_PIX_FMT_RGB32, video->codec_context->width, video->codec_context->height, 1);

//     QElapsedTimer timer;
//     timer.start();
//     AVRational tb = format_context->streams[video->stream_id]->time_base;

//     while (av_read_frame(format_context, packet) >= 0) {
//         if (packet->stream_index == video->stream_id) {
//             avcodec_send_packet(video->codec_context, packet);
//             while (avcodec_receive_frame(video->codec_context, frame) == 0) {
//                 int64_t pts = frame->best_effort_timestamp;
//                 qint64 targetMs = pts * 1000 * tb.num / tb.den;
//                 qint64 now = timer.elapsed();
//                 if (targetMs > now)
//                     QThread::msleep(targetMs - now);
//                 sws_scale(
//                     sws,
//                     frame->data, frame->linesize,
//                     0, video->codec_context->height,
//                     rgbFrame->data, rgbFrame->linesize
//                     );
//                 QImage image(rgbFrame->data[0], video->codec_context->width, video->codec_context->height, rgbFrame->linesize[0], QImage::Format_RGB32);
//                 QVideoFrame qframe(image.copy());
//                 new_frame_cnt+=1;
//                 QMetaObject::invokeMethod(this, [=]() {
//                     if (_sink)
//                         _sink->setVideoFrame(qframe);
//                     qDebug()<<new_frame_cnt;
//                 }, Qt::QueuedConnection);
//             }
//         }
//         av_packet_unref(packet);
//     }

//     av_frame_free(&frame);
//     av_frame_free(&rgbFrame);
//     av_packet_free(&packet);

// }

