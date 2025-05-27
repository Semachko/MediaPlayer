#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <queue>

template<typename T>
class Queue : QObject
{
public:
    Queue(qint64 size) : maxSize(size){}

    void push(const T& value){
        QMutexLocker locker(&mutex);
        queue.push(value);
    }
    void push(T&& value){
        QMutexLocker locker(&mutex);
        queue.push(std::move(value));
    }
    T pop(){
        QMutexLocker locker(&mutex);
        // if (queue.empty())
        //     return nullptr;
        T val = std::move(queue.front());
        queue.pop();
        return val;
    }
    T& back(){
        QMutexLocker locker(&mutex);
        // if (queue.empty())
        //     return T();
        return queue.back();
    }
    T& front(){
        QMutexLocker locker(&mutex);
        // if (queue.empty())
        //     return T();
        return queue.front();
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
        while (!queue.empty()) {
            queue.pop();
        }
    }

private:
    mutable QMutex mutex;
    std::queue<T> queue;
    qint64 maxSize = 2;
};
#endif // QUEUE_H
