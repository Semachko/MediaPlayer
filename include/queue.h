#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <mutex>
#include <queue>

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
            queue.push(value);
        }
        cv.notify_one();
    }
    void push(T&& value){
        {
            std::lock_guard _(mutex);
            queue.push(std::move(value));
        }
        cv.notify_one();
    }
    T try_pop() {
        std::lock_guard _(mutex);
        if(queue.empty())
            return T{};
        T data = queue.front();
        queue.pop();
        return data;
    }
    T wait_pop() {
        std::unique_lock lock(mutex);
        cv.wait(lock, [this] {return !queue.empty();});
        T data = queue.front();
        queue.pop();
        return data;
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
        std::queue<T> empty;
        std::swap(queue, empty);
    }

private:
    mutable std::mutex mutex;
    std::condition_variable cv;
    std::queue<T> queue;
    qint64 max_size;
};
#endif // QUEUE_H
