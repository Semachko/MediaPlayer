#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <QQueue>
#include <mutex>

template<typename T>
class Queue
{
public:
    Queue(qint64 size) : max_size(size){}
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    void push(const T& value){
        {
            std::lock_guard _(mutex);
            queue.enqueue(value);
        }
        cv.notify_one();
    }
    void push(T&& value){
        {
            std::lock_guard _(mutex);
            queue.enqueue(std::move(value));
        }
        cv.notify_one();
    }
    void push(QQueue<T>&& values) {
        if (values.empty())
            return;
        {
            std::lock_guard<std::mutex> _(mutex);
            for (auto& v : values)
                queue.enqueue(std::move(v));
        }
        cv.notify_all();
    }

    T try_pop() {
        std::lock_guard _(mutex);
        if(queue.empty())
            return T{};
        return queue.dequeue();
    }

    T wait_pop() {
        std::unique_lock lock(mutex);
        cv.wait(lock, [this] {return !queue.empty();});
        T data = queue.front();
        return queue.dequeue();
    }

    T& front() {
        std::unique_lock lock(mutex);
        cv.wait(lock, [this] {return !queue.empty();});
        return queue.front();
    }
    bool empty() const{
        std::lock_guard _(mutex);
        return queue.empty();
    }
    bool is_full() const{
        std::lock_guard _(mutex);
        return queue.size() >= max_size;
    }
    size_t size() const{
        std::lock_guard _(mutex);
        return queue.size();
    }
    void clear(){
        std::lock_guard _(mutex);
        queue.clear();
    }

private:
    mutable std::mutex mutex;
    std::condition_variable cv;
    QQueue<T> queue;
    qint64 max_size;
};
#endif // QUEUE_H
