#include "audioiodevice.h"
#include <qdebug.h>

AudioIODevice::AudioIODevice(QObject *parent) : QIODevice(parent) {
    open(QIODevice::ReadOnly);
}

void AudioIODevice::appendData(const QByteArray &data) {
    //qDebug()<<"Appending data";
    buffer.append(data);
    if (!isOpen())
        open(QIODevice::ReadOnly);
    if (buffer.size() >= 8192) {
        //qDebug()<<"Signaling to read data";
        if
        emit readyRead();
    }
}

qint64 AudioIODevice::readData(char *data, qint64 maxlen)
{
    //qDebug() << "readData requested" << maxlen << "bytes";
    qint64 len = qMin(maxlen, (qint64)buffer.size());
    memcpy(data, buffer.constData(), len);
    buffer.remove(0, len);
    return len;
}


qint64 AudioIODevice::writeData(const char *, qint64)
{
    return -1;
}

qint64 AudioIODevice::bytesAvailable() const
{
    //qDebug()<<"Buffer size = "<<buffer.size();
    //qDebug()<<"Buffer size + bytesAvailable = "<<buffer.size() + QIODevice::bytesAvailable();
    return buffer.size() + QIODevice::bytesAvailable();
}

