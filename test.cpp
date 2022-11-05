#include <iostream>
#include <string>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
/*
#include "log/my_time.h"
#include "log/log.h"
#include "log/block_queue.h"
#include "mysql/mysql_conn_pool.h"
*/
using namespace std;

int main()
{
    int serv_sock = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(4203);
    bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    listen(serv_sock,20);
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);

    char buf[1024]; /* recv buffer */
    char html[1024]; /* html buffer */

    FILE* index = fopen("./root/index.html","r");
    fread(html,sizeof(html),1,index);

    string tosend = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccess-Control-Allow-Origin: *\r\n\r\n"; /* Initialize as response head. */
    tosend += string(html); /* Add content after head. */
    printf("%s\n",tosend.c_str()); /* To see whether the response string is organized properly. */

    /* Save http request package from browser. */
    FILE* fp = fopen("http.txt","w");

    read(clnt_sock,buf,sizeof(buf));
    fputs(buf,fp); 

    /* Answer request. Send the response package to client. */
    write(clnt_sock,tosend.c_str(),sizeof(tosend.c_str()));
    
    
    fclose(fp);
    /* Close socket. */
    close(clnt_sock);
    close(serv_sock);

    printf("done.\n");
    return 0;
}  