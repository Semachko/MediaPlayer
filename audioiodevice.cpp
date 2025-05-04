#include "audioiodevice.h"
#include <qdebug.h>

AudioIODevice::AudioIODevice(Synchronizer* sync, QObject *parent) : sync(sync), QIODevice(parent) {
    open(QIODevice::ReadOnly);
}
constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

void AudioIODevice::appendData(const QByteArray &data) {
    //qDebug()<<OUTPUT<<"Appending sample data to output buffer";
    buffer.append(data);
    if (!isOpen())
        open(QIODevice::ReadOnly);
    if (buffer.size() >= 8192) {
        //sync->check_pause();
        //qDebug()<<OUTPUT<<"Signaling device to read data";
        emit readyRead();
    }
}

qint64 AudioIODevice::readData(char *data, qint64 maxlen)
{
    //qDebug() << OUTPUT <<"ReadData requested" << maxlen << "bytes";
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
    //qDebug()<<OUTPUT<<"Buffer size + bytesAvailable = "<<buffer.size() + QIODevice::bytesAvailable();
    return buffer.size() + QIODevice::bytesAvailable();
}

