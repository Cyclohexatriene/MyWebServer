#ifndef MYSQL_CONN_POOL_H
#define MYSQL_CONN_POOL_H

#include <iostream>
#include <list>
#include <mysql/mysql.h>
#include <string>
#include "../lock/locker.h"
#include "../log/log.h"

using namespace std;

class connection_pool
{
private:
    connection_pool();
    ~connection_pool();
    string user;
    string passwd;
    string DatabaseName;
    bool close_log; /* If true, the connection pool won't write logs. */
    int max_conn;
    locker m_locker;
    list<MYSQL*> connList;
    sem m_sem; /* sem > 0 means there is free connections. */
public:
    static connection_pool* get_instance()
    {
        static connection_pool instance;
        return &instance;
    }

    void init(string user, string password, string name, int maxconn, bool close_log = true);
    
    MYSQL* getConnection(); /* Get a free connection from pool. */
    bool releaseConnection(MYSQL *conn); /* Return a connection into the pool. */
    void destroyPool();
};


class connectionRAII
{
private:
    MYSQL* connRAII;
    connection_pool *poolRAII;
public:
    connectionRAII(MYSQL** conn, connection_pool *connPool);
    ~connectionRAII();
};

#endif