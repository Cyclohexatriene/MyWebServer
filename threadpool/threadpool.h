#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <pthread.h>

#include "../lock/locker.h"
#include "../log/log.h"
#include "../log/block_queue.h"
#include "../http/http.h"
#include "../mysql/mysql_conn_pool.h"

class threadpool
{
private:
    threadpool();
    ~threadpool();
    static void* worker(void* arg);/* Call process() in an infinite loop. */

private:
    pthread_t* m_threads;
    block_queue<http*> workq;
    int m_thread_number;
    sem m_sem;
    locker m_locker;
public:
    bool append(http* request);
    void init(int thread_number, int max_requests);
    static threadpool* get_instance(){
        static threadpool instance;
        return &instance;
    }
};

#endif