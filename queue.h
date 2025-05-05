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
    Queue() = default;
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
        if (queue.empty())
            throw std::exception("Queue is empty!");
        T val = std::move(queue.front());
        queue.pop();
        return val;
    }
    bool empty() const{
        QMutexLocker locker(&mutex);
        return queue.empty();
    }
    size_t size() const{
        QMutexLocker locker(&mutex);
        return queue.size();
    }
    // void clear(){
    //     QMutexLocker locker(&mutex);
    //     std::queue<T> empty;
    //     std::swap(queue, empty);
    // }

    int max_size = 0;
private:
    mutable QMutex mutex;
    std::queue<T> queue;
};
#endif // QUEUE_H
