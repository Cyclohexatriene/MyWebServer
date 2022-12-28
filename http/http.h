#ifndef HTTP_H
#define HTTP_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <sys/uio.h>


using namespace std;

class http{
private:
    char state_line[64];
    char headers[256];
    int state; /* 0: wait for process; 1: ready to write; 2: bad request */

public:
    http();
    http(char pack[], int fd);
    ~http();
    void process();
    int clntfd;

private:
    void response(char* filepath);

public:
    char package[1024];
    string package_str;
    struct iovec iov[3]; /* iov[0]: state_line; iov[1]: headers; iov[2]: resource to be got */
};

#endif