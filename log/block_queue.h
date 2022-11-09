/* Encapsule std::queue as a block_queue */

#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <iostream>
#include <queue>
#include "../lock/locker.h"

template<typename T>
class block_queue
{
private:
    locker m_locker;
    cond m_cond;
    queue<T> q;
    int m_max_size;
public:
    
    block_queue(int max_size = 1000)
    {
        if(max_size <= 0)
            throw exception();
        m_max_size = max_size;
    };

    ~block_queue(){};

    void clear()
    {
        m_locker.lock();
        queue<T> empty;
        swap(empty,q);
        m_locker.unlock();
    }

    bool full()
    {
        m_locker.lock();
        if(q.size() >= m_max_size)
        {
            m_locker.unlock();
            return true;
        }
        m_locker.unlock();
        return false;
    }

    bool empty()
    {
        m_locker.lock();
        if(q.empty())
        {
            m_locker.unlock();
            return true;
        }
        m_locker.unlock();
        return false;
    }

    bool front(T &value)
    {
        m_locker.lock();
        if(q.empty())
        {
            m_locker.unlock();
            return false;
        }
        value = q.front();
        m_locker.unlock();
        return true;
    }

    bool back(T &value)
    {
        m_locker.lock();
        if(q.empty())
        {
            m_locker.unlock();
            return false;
        }
        value = q.back();
        m_locker.unlock();
        return true;
    }
    
    int size()
    {
        int res = 0;
        m_locker.lock();
        res = q.size();
        m_locker.unlock();
        return res;
    }

    bool push(const T &item)
    {
        m_locker.lock();
        while(q.size() >= m_max_size)
        {
            m_cond.broadcast();
            m_locker.unlock();
            return false;
        }

        q.emplace(item);
        m_cond.broadcast();
        m_locker.unlock();
        printf("push successful.\n");
        return true;
    }

    bool pop(T &item)
    {
        m_locker.lock();
        while(q.empty())
        {
            if(!m_cond.wait(m_locker.get()))
            {
                m_locker.unlock();
                return false;
            }
        }

        item = q.front();
        q.pop();
        m_locker.unlock();
        return true;
    }
};



#endif