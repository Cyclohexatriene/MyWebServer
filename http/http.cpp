#include "http.h"
#include "../mmap/mmap_file.h"
#include <string.h>
#include <regex>

http::http(){}

http::http(char pack[],int fd){
    memcpy(package,pack,strlen(pack));
    package_str = string(package);
    clntfd = fd;
    state = 0;
}

void http::process()
{
    
    /* Organize answer package according to request type. */
    char request_type[5];
    strncpy(request_type,package,3);
    request_type[3] = '\0';
    if(strcmp(request_type,"GET") == 0){
        char request_path[128];
        int idx = 4;
        while(package[idx]!=' '){
            request_path[idx-4] = package[idx];
            idx++;
        }
        request_path[idx-4] = '\0';
        if(strcmp(request_path,"/") == 0) strcpy(request_path,"/index.html");
        //mmap_file file("./root/index.html");
        char filepath[128] = "./root";
        strcat(filepath,request_path);
        FILE* file = fopen(filepath,"r");
        char f[1024];
        if(file == nullptr){
            file = fopen("./root/404notfound.html","r");
        }

        strcpy(state_line,"HTTP/1.1 200 OK\r\n");
        strcpy(headers,"Access-Control-Allow-Origin: *\r\nContent-Type: text/html\r\nContent-Length: ");
        fread(f, sizeof(f),1,file);
        fclose(file);
        char len[16];
        sprintf(len,"%ld",strlen(f));
        strcat(headers,len);
        strcat(headers,"\r\n");
        strcat(headers,"\r\n");

        
        iov[0].iov_base = (void*)state_line;
        iov[0].iov_len = strlen(state_line);
        iov[1].iov_base = (void*)headers;
        iov[1].iov_len = strlen(headers);
        iov[2].iov_base = (void*)f;
        iov[2].iov_len = strlen(f);
        state = 1;
    }
    else{
        printf("%s\n",request_type);
        strncpy(request_type,package,4);
        request_type[4] = '\0';
        if(strcmp(request_type,"POST") == 0){
            
            smatch m;
            regex e("\r\n\r\n");
            regex_search(package_str, m, e);
            cout << m.suffix().str() << endl;
            state = 1;
        }
        else{
            state = 2;
        }
    }
}