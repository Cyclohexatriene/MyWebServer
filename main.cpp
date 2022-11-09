#include <iostream>
#include <string>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unordered_map>
#include <errno.h>

#include "log/my_time.h"
#include "log/log.h"
#include "log/block_queue.h"
#include "mysql/mysql_conn_pool.h"
#include "epoll/epoll.h"
#include "http/http.h"
#include "threadpool/threadpool.h"

using namespace std;

int main()
{
    int serv_sock = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(4203);
    int flag = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    printf("the serv_sock is: %d\n",serv_sock);
    listen(serv_sock,20);
    epoll::get_instance() -> init();
    epoll_event serv_event;
    serv_event.events = EPOLLIN | EPOLLET;
    serv_event.data.fd = serv_sock;
    epoll::get_instance() -> addfd(serv_sock,&serv_event);
    printf("\nListening...\n");
    unordered_map<int,http*> fd2http; /* Mapping client fd to its own http object. */

    threadpool::get_instance() -> init(5,5000);

    while(1)
    {   
        epoll_event events[MAX_EVENT];
        int n = epoll::get_instance() -> wait(events);
        printf("looping...\n");
        for(int i = 0; i < n; i++)
        {
            epoll_event ev = events[i];
            printf("fd = %d\n",ev.data.fd);
            if(ev.data.fd == serv_sock)
            {
                printf("epoll triggered.\n");
                int clntfd = accept(serv_sock,NULL,NULL);
                printf("Received a request.\n");
                http* conn = new http();
                conn -> clntfd = clntfd;
                printf("clntfd is: %d\n",clntfd);
                fd2http[clntfd] = conn;
                epoll::get_instance() -> addfd_wait4read(clntfd);
                
            }
            else if(ev.events & EPOLLIN)
            {
                printf("Received a package.\n");
                http* conn = fd2http[ev.data.fd];
                printf("%s\n",conn -> package);
                read(ev.data.fd, conn -> package, sizeof(conn -> package));
                printf("%s\n",conn -> package);
                printf("Datagram got.\n");
                threadpool::get_instance() -> append(conn);
            }
            else if(ev.events & EPOLLOUT)
            {
                printf("Sending package...\n");
                http* conn = fd2http[ev.data.fd];
                
                int byte_sent = writev(ev.data.fd, conn -> iov, sizeof(conn -> iov)/sizeof(iovec));
                printf("Datagram sent, bytes: %d\n",byte_sent);
                printf("errno: %d\n",errno);
                //printf("%s%s%s\n",(char*)conn -> iov[0].iov_base,(char*)conn->iov[1].iov_base,(char*)conn->iov[2].iov_base);
                
                /*
                char towrite[1024];
                strcat(towrite,(char*) (conn -> iov[0].iov_base));
                strcat(towrite,(char*) (conn -> iov[1].iov_base));
                strcat(towrite,(char*) (conn -> iov[2].iov_base));
                printf("%s\n",towrite);
                printf("%ld\n",strlen(towrite));
                write(conn -> clntfd, towrite,strlen(towrite));
                */
                epoll::get_instance() -> addfd_wait4read(ev.data.fd);

                /* TODO: mmap & writev */
            }
        }
    }

}  