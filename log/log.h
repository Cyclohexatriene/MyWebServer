#ifndef LOG_H
#define LOG_H

#include <iostream>
#include "my_time.h"
#include "../lock/locker.h"
#include "block_queue.h"
using namespace std;

#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_ERROR 2
class Log
{
private:
    string dir;
    string fileName;
    locker m_locker;
    int max_line;
    int cnt_line;
    bool is_async;
    my_time t;
    string date;
    int seq; /* Sequence of logs in one day. */
    FILE *fp;
    block_queue<string> *log_q;
private:
    Log();
    ~Log();
    void async_write_log()
    {
        string single_log;
        while(log_q -> pop(single_log))
        {
            if(!m_locker.lock())
                throw exception();

            fputs(single_log.c_str(), fp);

            if(!m_locker.unlock())
                throw exception();
        }
    }
public:
    static Log *get_instance()
    {
        static Log instance;
        return &instance;
    }

    static void *flush_log_thread(void *argv)
    {
        Log::get_instance()->async_write_log();
        return (void*)0;
    }

    bool init(int max_line = 5000000, int max_q_size = 0);

    void write_log(int level, const string& content);

    void flush();
    
};

#endif