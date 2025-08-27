#ifndef IMEDIACONTEXT_H
#define IMEDIACONTEXT_H

#include <QObject>
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
        codec(stream),
        timeBase(codec.stream->time_base)
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
    AVRational timeBase;
    Queue<Packet> packet_queue;
};

#endif // IMEDIACONTEXT_H
