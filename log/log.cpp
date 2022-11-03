#include "log.h"
#include <unistd.h>
Log::Log()
{
    cnt_line = 0;
    seq = 0;
    dir = "server_logs/";
    is_async = false;
};

Log::~Log()
{
    if(fp != NULL)
        fclose(fp);
};

bool Log::init(int max_line, int max_q_size)
{
    if(max_q_size >= 1)
    {
        /* If max_q_size is set, switch to asynchronous mode. */
        is_async = true;
        log_q = new block_queue<string>(max_q_size);
        pthread_t pid;
        pthread_create(&pid, NULL, flush_log_thread, NULL);
    }

    /* The max_q_size is not set, the log work at synchronous mode. */
    this -> max_line = max_line;
    fileName = t.str();
    fileName += "_";
    fileName += to_string(seq);
    date = t.str().substr(0,10);
    fp = fopen((dir+fileName).c_str(), "a");
    if(fp == NULL)
        return false;
    
    return true;
}

void Log::write_log(int level, const string& content)
{
    if(!m_locker.lock())
        throw exception();
    
    /* Check whether should open a new log. */
    t.getTime();
    if(t.str().substr(0,10) != date || cnt_line >= max_line)
    {
        fflush(fp);
        fclose(fp);
        if(t.str().substr(0,10) != date)
        {
            seq = 0;
            cnt_line = 0;
        }
        while(cnt_line >= max_line)
        {
            seq++;
            cnt_line -= max_line;
        }
        string newFileName = t.str();
        newFileName += "_";
        newFileName += to_string(seq);
        fp = fopen((dir+newFileName).c_str(), "a");
    }

    /* Generate log content and write. */
    string log_content;
    switch(level)
    {
    case LOG_DEBUG:
        log_content = "[Debug]";
        break;
    case LOG_INFO:
        log_content = "[Info]";
        break;
    case LOG_ERROR:
        log_content = "[Error]";
        break;
    }
    log_content += t.str();
    log_content += ": ";
    log_content += content;
    log_content +="\n";
    
    if(!m_locker.unlock())
        throw exception();

    cnt_line++;
    
    if(is_async && !log_q -> full())
    {
        log_q -> push(log_content);
        
    }
    else
    {
        if(!m_locker.lock())
            throw exception();

        fputs(log_content.c_str(),fp);
        
        if(!m_locker.unlock())
            throw exception();
    }
}

void Log::flush()
{
    if(!m_locker.lock())
        throw exception();

    fflush(fp);

    if(!m_locker.unlock())
        throw exception();
        
}