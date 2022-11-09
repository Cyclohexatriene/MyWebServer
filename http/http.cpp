#include "http.h"
#include "../mmap/mmap_file.h"
#include <string.h>

http::http(){}

http::http(char pack[],int fd){
    memcpy(package,pack,strlen(pack));
    clntfd = fd;
}

void http::process()
{
    strcpy(state_line,"HTTP/1.1 200 OK\r\n");
    strcpy(headers,"Access-Control-Allow-Origin: *\r\nContent-Type: text/html\r\nContent-Length: ");
    //mmap_file file("./root/index.html");
    FILE* file = fopen("./root/index.html","r");
    char f[1024];
    fread(f, sizeof(f),1,file);
    fclose(file);
    char len[16];
    sprintf(len,"%ld",strlen(f));
    strcat(headers,len);
    strcat(headers,"\r\nConnection: close\r\n");
    strcat(headers,"\r\n");
    iov[0].iov_base = (void*)state_line;
    iov[0].iov_len = strlen(state_line);
    iov[1].iov_base = (void*)headers;
    iov[1].iov_len = strlen(headers);
    iov[2].iov_base = (void*)f;
    iov[2].iov_len = strlen(f);
}