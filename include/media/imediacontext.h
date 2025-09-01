#ifndef IMEDIACONTEXT_H
#define IMEDIACONTEXT_H

#include <QObject>
#include <qdebug.h>
#include <queue.h>
#include <packet.h>
#include "media/codec.h"

class IMediaContext : public QObject
{
    Q_OBJECT
public:
    IMediaContext(AVStream* stream, qint64 queue_size)
        :
        packet_queue(queue_size),
        codec(stream)
    {}
    virtual void process_packet() = 0;
    virtual qint64 buffer_available() = 0;
signals:
    void requestPacket();
    void newPacketArrived();

protected:
    qint64 maxBufferSize;
public:
    Codec codec;
    Queue<Packet> packet_queue;
};

#endif // IMEDIACONTEXT_H
