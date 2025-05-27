#include "audioiodevice.h"
#include <qdebug.h>

AudioIODevice::AudioIODevice(Synchronizer* sync, QObject *parent)
    :
    QIODevice(parent),
    sync(sync)
{
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

void AudioIODevice::clear()
{
    QMutexLocker _(&clearAvailable);
    buffer.clear();
}

qint64 AudioIODevice::readData(char *data, qint64 maxlen)
{
    sync->check_pause();

    QMutexLocker _(&clearAvailable);
    qint64 len = qMin(maxlen, (qint64)buffer.size());
    if (len > 0) {
        memcpy(data, buffer.constData(), len);
        buffer.remove(0, len);
        emit dataReaded();
    }
    return len;
}
qint64 AudioIODevice::writeData(const char *, qint64)
{
    return -1;
}

qint64 AudioIODevice::bytesAvailable() const
{
    return buffer.size() + QIODevice::bytesAvailable();
}


AudioIODevice::~AudioIODevice()
{
}
