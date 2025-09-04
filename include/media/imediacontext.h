#ifndef IMEDIACONTEXT_H
#define IMEDIACONTEXT_H

#include <QObject>
#include <qdebug.h>
#include <queue.h>
#include <packet.h>
#include "media/codec.h"
#include "media/decoder.h"

class IMediaContext : public QObject
{
    Q_OBJECT
public:
    virtual ~IMediaContext() = default;
    IMediaContext(AVStream* stream, qint64 queue_size)
        :
        packet_queue(queue_size),
        codec(stream),
        decoder(codec)
    {
        connect(this,&IMediaContext::newPacketArrived, this, &IMediaContext::process_packet);
    }

    virtual void process_packet() = 0;
    virtual qint64 buffer_available() = 0;
signals:
    void requestPacket();
    void newPacketArrived();
    void endReached();

protected:
    qint64 maxBufferSize;
public:
    Codec codec;
    Decoder decoder;
    Queue<Packet> packet_queue;
};

#endif // IMEDIACONTEXT_H
