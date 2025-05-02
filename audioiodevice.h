#ifndef AUDIOIODEVICE_H
#define AUDIOIODEVICE_H

#include <QIODevice>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>


class AudioIODevice : public QIODevice {
    Q_OBJECT
public:
    explicit AudioIODevice(QObject* parent = nullptr);

    void appendData(const QByteArray& data);
protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char*, qint64) override;
    qint64 bytesAvailable() const override;

public:

private:
    QByteArray buffer;
    int queueSize = 10;
};

#endif // AUDIOIODEVICE_H
