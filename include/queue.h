#ifndef QUEUE_H
#define QUEUE_H

#include <QObject>
#include <QQueue>
#include <mutex>
#include <queue>

template<typename T>
class Queue
{
public:
    Queue(qint64 size = 1) : full_size(size){}
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
            queue.emplace(std::move(value));
        }
        cv.notify_one();
    }
    void push(QQueue<T>&& values) {
        if (values.empty())
            return;
        {
            std::lock_guard<std::mutex> _(mutex);
            for (auto& v : values)
                queue.emplace(std::move(v));
        }
        cv.notify_all();
    }

    T try_pop() {
        std::lock_guard _(mutex);
        if(queue.empty())
            return T{};
        T result = queue.front();
        queue.pop();
        return result;
    }

    T wait_pop() {
        std::unique_lock lock(mutex);
        cv.wait(lock, [this] {return !queue.empty();});
        T result = queue.front();
        queue.pop();
        return result;
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
        return queue.size() >= full_size;
    }
    size_t size() const{
        std::lock_guard _(mutex);
        return queue.size();
    }
    void clear(){
        std::lock_guard _(mutex);
        while(!queue.empty())
            queue.pop();
    }
    void set_full_size(qint64 new_full_size){
        std::lock_guard _(mutex);
        full_size = new_full_size;
    }
    qint64 get_full_size(){
        std::lock_guard _(mutex);
        return full_size;
    }
private:
    mutable std::mutex mutex;
    std::condition_variable cv;
    std::queue<T> queue;
    qint64 full_size{1};
};
#endif // QUEUE_H
