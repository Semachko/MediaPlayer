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

    void clear();
    void append(const QByteArray& data);
    qint64 bytesAvailable() const override;
protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
signals:
    void dataReaded();
///////////////////////////////////////////////
///////////////////////////////////////////////
private:
    mutable QMutex mutex;
    QByteArray buffer;
};

#endif // AUDIOIODEVICE_H
