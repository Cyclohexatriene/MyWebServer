#ifndef EPOLL_H
#define EPOLL_H

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#define MAX_EVENT 10000

using namespace std;


class epoll{
private:
    epoll(){};
    ~epoll(){
        close(epollfd);
    };

private:
    int epollfd;

public:
    void init()
    {
        epollfd = epoll_create(5);
    }

    static epoll* get_instance()
    {
        static epoll instance;
        return &instance;
    }

    int getfd()
    {
        return epollfd;
    }

    int addfd_wait4read(int fd)
    {
        epoll_event event;
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        event.data.fd = fd;
        //printf("adding read fd: %d\n",fd);
        return epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    }

    int addfd_wait4write(int fd)
    {
        epoll_event event;
        event.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
        event.data.fd =fd;
        //printf("adding write fd: %d\n",fd);
        bool flag = epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
        return flag;
    }

    int addfd(int fd, epoll_event* event)
    {
        //printf("adding fd: %d\n",fd);
        return epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, event);
    }

    int wait(epoll_event* events){
        return epoll_wait(epollfd,events,MAX_EVENT,-1);
    }
};

#endif