#ifndef IMEDIACONTEXT_H
#define IMEDIACONTEXT_H

#include <QObject>
#include <queue.h>
#include <packet.h>

class IMediaContext : public QObject
{
    Q_OBJECT
public:
    IMediaContext(int queue_capacity) : packetQueue(queue_capacity) {}

    virtual void push_frame_to_buffer() = 0;
signals:
    void requestPacket();
    void newPacketArrived();

public:
    Queue<Packet> packetQueue;
};

#endif // IMEDIACONTEXT_H
