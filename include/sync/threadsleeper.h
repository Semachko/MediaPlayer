#ifndef THREADSLEEPER_H
#define THREADSLEEPER_H

#include <QObject>
#include <mutex>

class ThreadSleeper
{
public:
    ThreadSleeper();
    void wait(qreal seconds);
    void wake();

private:
    std::mutex mutex;
    std::condition_variable cv;
    bool waked = false;
};

#endif // THREADSLEEPER_H
