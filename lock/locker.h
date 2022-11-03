#ifndef LOCKER_H
#define LOCKER_H

#include <iostream>
#include <pthread.h>
using namespace std;

class locker
{
private:
    pthread_mutex_t mutex;
public:
    locker();
    ~locker();
    bool lock();
    bool unlock();
    pthread_mutex_t* get();
};

class cond
{
private:
    pthread_cond_t m_cond;
public:
    cond();
    ~cond();
    bool wait(pthread_mutex_t *mutex);
    bool signal();
    bool broadcast();
};

#endif