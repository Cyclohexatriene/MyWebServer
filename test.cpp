#include <iostream>
#include <pthread.h>
#include "log/block_queue.h"
#include <unistd.h>
#include <vector>
#include "log/my_time.h"
#include "log/log.h"
#include "mysql/mysql_conn_pool.h"
using namespace std;

void* worker(void*)
{
    
    for(int i=0;i<10;i++)
    {
        MYSQL* conn;
        connectionRAII connRAII(&conn,connection_pool::get_instance());
        if(conn == NULL)
        {
            Log::get_instance() -> write_log(LOG_DEBUG,"connRAII failed.");
        }
        string sql = "INSERT INTO test values('user";
        sql += to_string(i);
        sql += "','123456')";
        mysql_query(conn,sql.c_str());
    }
    return (void*)0;
}

int main()
{
    connection_pool::get_instance() -> init("root","","test",10,false);
    Log::get_instance() -> init();
   
    vector<pthread_t> vec(10);
    for(auto i : vec) pthread_create(&i,NULL,worker,NULL);
    for(auto i : vec) pthread_join(i,NULL);
/*
    pthread_t pid;
    pthread_create(&pid,NULL,worker,NULL);
    pthread_join(pid,NULL);
*/
    sleep(2);
    printf("done.\n");
}  