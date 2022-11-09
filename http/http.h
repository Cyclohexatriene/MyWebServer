#ifndef HTTP_H
#define HTTP_H

#include <iostream>
#include <string>
#include <sys/uio.h>


using namespace std;

class http{
public: /*private*/
    char state_line[64];
    char headers[256];
public:
    http();
    http(char pack[], int fd);
    ~http();
    void process();
    int clntfd;

public:
    char package[1024];
    struct iovec iov[3]; /* iov[0]: state_line; iov[1]: headers; iov[2]: resource to be got */
};

#endif