#ifndef ICLOCK_H
#define ICLOCK_H
#include <QObject>

class IClock
{
public:
    virtual ~IClock() = default;
    IClock() {};
    virtual qreal get_time() = 0;
    virtual void set_time(qreal seconds) = 0;
};

#endif // ICLOCK_H
