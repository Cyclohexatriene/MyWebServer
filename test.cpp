#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>

#include "./epoll/epoll.h"
#include "./http/http.h"

using namespace std;

int main(){

    unordered_map<int,char*> map;

    /* Create a socket that listening. */
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(4203);
    bind(listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    listen(listenfd,20);

    /* Create an epoll, and register listenfd. */
    int epollfd = epoll_create(5);
    epoll_event* event;
    event -> events = EPOLLIN | EPOLLET;
    event -> data.fd = listenfd;
    printf("adding fd: %d\n",listenfd);
    epoll_ctl(epollfd,EPOLL_CTL_ADD, listenfd,event);

    /* Wait for client connection. */
    int event_num = 5;
    epoll_event events[event_num];
    int n = epoll_wait(epollfd, events, event_num,-1);
    int clntfd;
    for(int i = 0; i < n ; i++)
    {
        epoll_event ev = events[i];
        clntfd = accept(ev.data.fd,NULL,NULL);
        char* str = new char[12];
        strcpy(str,"hello,world");
        map[clntfd] = str;
    }
    
    /* Register client socket. */
    epoll_event* clnt_ev;
    clnt_ev -> events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    clnt_ev -> data.fd = clntfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, clntfd,clnt_ev);

    /* Wait for client requests. */
    n = epoll_wait(epollfd, events,event_num,-1);
    for(int i = 0; i < n; i++)
    {
        epoll_event ev = events[i];
        if(ev.data.fd == listenfd) continue;
        printf("%s\n",map[ev.data.fd]);
    }


}