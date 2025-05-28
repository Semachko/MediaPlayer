#include "audioiodevice.h"
#include <qdebug.h>

constexpr auto OUTPUT = "\033[34m[Output]\033[0m";

AudioIODevice::AudioIODevice(QObject *parent)
    :
    QIODevice(parent)
{
    open(QIODevice::ReadOnly);
}

void AudioIODevice::append(const QByteArray &data)
{
    QMutexLocker _(&mutex);
    buffer.append(data);
}
qint64 AudioIODevice::readData(char *data, qint64 maxlen)
{
    QMutexLocker _(&mutex);
    qint64 len = qMin(maxlen, (qint64)buffer.size());
    if (len > 0) {
        memcpy(data, buffer.constData(), len);
        buffer.remove(0, len);
        emit dataReaded();
    }
    return len;
}
qint64 AudioIODevice::bytesAvailable() const
{
    QMutexLocker _(&mutex);
    return buffer.size() + QIODevice::bytesAvailable();
}
void AudioIODevice::clear()
{
    QMutexLocker _(&mutex);
    buffer.clear();
}


qint64 AudioIODevice::writeData(const char *data, qint64 maxSize)
{
    return -1;
}
