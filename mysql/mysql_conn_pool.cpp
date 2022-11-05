#include "mysql_conn_pool.h"

connection_pool::connection_pool()
{
    max_conn = 0;
}

void connection_pool::init(string user, string password, string DatabaseName, int maxconn, bool close_log)
{
    this -> user = user;
    this -> passwd = password;
    this -> DatabaseName = DatabaseName;
    this -> max_conn = maxconn;
    this -> close_log = close_log;
    m_sem = sem();
    for(int i=0; i<max_conn; i++)
    {
        MYSQL* conn = NULL;
        conn = mysql_init(conn);
        if(conn == NULL)
        {
            if(!close_log) Log::get_instance() -> write_log(LOG_ERROR,"MySQL error");
            exit(-1);
        }
        conn = mysql_real_connect(conn,"localhost",user.c_str(),passwd.c_str(),DatabaseName.c_str(),0,NULL,0);
        if(conn == NULL)
        {
            if(!close_log) Log::get_instance() -> write_log(LOG_ERROR,"MySQL error.");
            exit(-1);
        }
        connList.push_back(conn);
        m_sem.post();
    }
}

MYSQL* connection_pool::getConnection()
{
    MYSQL *conn = NULL;
    if(connList.size() == 0)
        return NULL;

    m_sem.wait();

    m_locker.lock();

    conn = connList.front();
    connList.pop_front();
    //if(!close_log) Log::get_instance() -> write_log(LOG_DEBUG,"get connection successful.");

    m_locker.unlock();
    return conn;
}

bool connection_pool::releaseConnection(MYSQL *conn)
{
    if(conn == NULL)
        return false;

    m_locker.lock();

    connList.push_back(conn);
    //if(!close_log) Log::get_instance() -> write_log(LOG_DEBUG,"return connection seccessful.");
    m_locker.unlock();
    m_sem.post();
    return true;
}

void connection_pool::destroyPool()
{
    m_locker.lock();
    if(connList.size() > 0)
    {
        for(auto i : connList)
            mysql_close(i);
    }
    m_locker.unlock();
}

connection_pool::~connection_pool()
{
    destroyPool();
}

connectionRAII::connectionRAII(MYSQL **conn, connection_pool *connPool)
{
    *conn = connPool -> getConnection();
    connRAII = *conn;
    poolRAII = connPool;
}

connectionRAII::~connectionRAII()
{
    poolRAII -> releaseConnection(connRAII);
}