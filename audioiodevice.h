#ifndef AUDIOIODEVICE_H
#define AUDIOIODEVICE_H

#include <QIODevice>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>

#include "synchronizer.h"

class AudioIODevice : public QIODevice {
    Q_OBJECT
public:
    explicit AudioIODevice(Synchronizer* sync, QObject* parent = nullptr);

    void appendData(const QByteArray& data);
    void clear();
    qint64 bytesAvailable() const override;
protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char*, qint64) override;
signals:
    void dataReaded();
///////////////////////////////////////////////
///////////////////////////////////////////////
public:
    const int max_buffer_size = 163840;
    QByteArray buffer;
private:
    Synchronizer* sync;
    mutable QMutex clearAvailable;
};

#endif // AUDIOIODEVICE_H
