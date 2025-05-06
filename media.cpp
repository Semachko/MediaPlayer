#include "media.h"

Media::Media() {
    mediacontext = new MediaContext();
    mediaThread = new QThread(this);
    mediacontext->moveToThread(mediaThread);
    mediaThread->start();

}

QVideoSink *Media::videoSink() const
{
    return videosink;
}

void Media::setVideoSink(QVideoSink *sink)
{
    if (videosink == sink)
        return;
    videosink = sink;
}

void Media::setFile(const QUrl &filename)
{
    if (mediacontext->video)
        disconnect(connection);
    //emit mediacontext.fileChanged(filename);
    emit mediacontext->fileChanged(filename,videosink);
}

void Media::output_image(QVideoFrame frame)
{
    //qDebug()<<"\033[32m[Screen]\033[0m Outputing image, size = "<<frame.size();
    videosink->setVideoFrame(frame);
}

void Media::playORpause()
{
    // if (!connection)
    //     connect(mediacontext->video->output, &FrameOutput::imageToOutput, this, &Media::output_image);
    emit mediacontext->playORpause();
}

void Media::muteORunmute()
{
    emit mediacontext->muteORunmute();
}

void Media::volumeChanged(qreal volume)
{
    emit mediacontext->volumeChanged(volume);
}

void Media::timeChanged(qreal time)
{
    emit mediacontext->timeChanged(time);
}

void Media::add5sec()
{

}

void Media::subtruct5sec()
{

}

void Media::repeatMedia()
{

}

void Media::shuffleMedia()
{

}


