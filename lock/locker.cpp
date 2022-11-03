#include "locker.h"

locker::locker()
{
    if(pthread_mutex_init(&mutex,NULL) != 0)
        throw exception();
}

locker::~locker()
{
    pthread_mutex_destroy(&mutex);
}

bool locker::lock()
{
    return pthread_mutex_lock(&mutex) == 0;
}

bool locker::unlock()
{
    return pthread_mutex_unlock(&mutex) == 0;
}

pthread_mutex_t* locker::get()
{
    return &mutex;
}


cond::cond()
{
    if(pthread_cond_init(&m_cond,NULL) != 0)
        throw exception();
}

cond::~cond()
{
    pthread_cond_destroy(&m_cond);
}

bool cond::wait(pthread_mutex_t *mutex)
{
    int ret = 0;
    ret = pthread_cond_wait(&m_cond,mutex);
    return ret == 0;
}

bool cond::signal()
{
    return pthread_cond_signal(&m_cond) == 0;
}

bool cond::broadcast()
{
    return pthread_cond_broadcast(&m_cond) == 0;
}