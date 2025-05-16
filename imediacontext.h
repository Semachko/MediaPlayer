#ifndef IMEDIACONTEXT_H
#define IMEDIACONTEXT_H

#include <QObject>
#include <queue.h>
#include <packet.h>

class IMediaContext : public QObject
{
    Q_OBJECT
public:
    IMediaContext(qint64 queue_size)
        : packetQueue(queue_size)
    {}

    virtual void decode_and_output() = 0;
    virtual qint64 buffer_available() = 0;
signals:
    void requestPacket();
    void newPacketArrived();

protected:
    qint64 maxBufferSize;
public:
    AVRational timeBase;
    Queue<Packet> packetQueue;
};

#endif // IMEDIACONTEXT_H
