#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <queue>

template<typename T>
class Queue : QObject
{
public:
    Queue(qint64 size) : maxSize(size){}

    void push(const T& value){
    {
        {
            QMutexLocker locker(&mutex);
            queue.push(value);
        }
        image_ready.notify_one();
    }
    }
    void push(T&& value){
        {
            QMutexLocker locker(&mutex);
            queue.push(std::move(value));
        }
        image_ready.notify_one();
    }
    T pop() {
        mutex.lock();
        while(queue.empty())
            image_ready.wait(&mutex);
        T data = queue.front();
        queue.pop();
        mutex.unlock();
        return data;
    }
    bool front(T& out) const {
        QMutexLocker locker(&mutex);
        if (queue.empty())
            return false;
        out = queue.front();
        return true;
    }
    bool back(T& out) const {
        QMutexLocker locker(&mutex);
        if (queue.empty())
            return false;
        out = queue.back();
        return true;
    }
    bool empty() const{
        QMutexLocker locker(&mutex);
        return queue.empty();
    }
    bool is_full() const{
        QMutexLocker locker(&mutex);
        return queue.size() >= maxSize;
    }
    size_t size() const{
        QMutexLocker locker(&mutex);
        return queue.size();
    }
    void clear(){
        QMutexLocker locker(&mutex);
        std::queue<T> empty;
        std::swap(queue, empty);
    }

private:
    mutable QMutex mutex;
    QWaitCondition image_ready;
    std::queue<T> queue;
    qint64 maxSize = 2;
};
#endif // QUEUE_H
