#ifndef THREADWAITER_H
#define THREADWAITER_H

#include <QObject>
#include <mutex>

class ThreadWaiter
{
public:
    ThreadWaiter();
    void wait(qreal seconds);
    void wake();

private:
    std::mutex mutex;
    std::condition_variable cv;
    bool waked = false;
};

#endif // THREADWAITER_H
