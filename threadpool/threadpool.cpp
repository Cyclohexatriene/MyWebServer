#include "threadpool.h"
#include "../epoll/epoll.h"

threadpool::threadpool(){};

void threadpool::init(int thread_number, int max_requests)
{
    workq = block_queue<http*>(max_requests);
    m_thread_number = thread_number;
    m_threads = new pthread_t[m_thread_number];
    for(int i=0;i<thread_number;i++)
    {
        pthread_create(&m_threads[i],NULL,worker,NULL);
    }
}

threadpool::~threadpool()
{
    for(int i=0;i < m_thread_number;i++)
    {
        pthread_cancel(m_threads[i]);
    }
    delete m_threads;
}

bool threadpool::append(http* request)
{
    bool flag = workq.push(request);
    m_sem.post();
    return flag;
}

void* threadpool::worker(void* arg)
{
    threadpool* pool = threadpool::get_instance();
    while(1)
    {
        pool -> m_sem.wait();
        pool -> m_locker.lock();
        if(pool -> workq.empty())
        {
            pool -> m_locker.unlock();
            continue;
        }
        http* request;
        pool -> workq.pop(request);
        pool -> m_locker.unlock();
        request -> process();
        epoll::get_instance() -> addfd_wait4write(request -> clntfd);
        printf("work complete.\n");
    }
}