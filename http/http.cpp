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
        char f[131072];
        if(file == nullptr){
            strcpy(filepath,"./root/404notfound.html");
            file = fopen(filepath,"r");
        }
        printf("path = %s\n",filepath);
        char contentType[64];
        idx = 0;
        for(int i = 0;i < strlen(filepath);i++){
            if(filepath[i] == '.') idx = 0;
            else{
                contentType[idx] = filepath[i];
                idx++;
            }
        }
        contentType[idx] = '\0';

        strcpy(state_line,"HTTP/1.1 200 OK\r\n");
        strcpy(headers,"Access-Control-Allow-Origin: *\r\nConnetion: close\r\nContent-Type: ");
        
        if(strcmp(contentType,"html") == 0) strcat(headers,"text/html\r\nContent-Length: ");
        else if(strcmp(contentType,"txt") == 0) strcat(headers,"text/plain\r\nContent-Length: ");
        else if(strcmp(contentType,"jpg") == 0) strcat(headers,"image/jpeg\r\nContent-Length: ");
        else if(strcmp(contentType,"png") == 0) strcat(headers,"image/png\r\nContent-Length: ");
        else if(strcmp(contentType,"ico") == 0) strcat(headers,"image/ico\r\nContent-Length: ");
        else strcat(headers,"text/html\r\nContent-Length: ");

        int nByte = fread(f, 1, sizeof(f),file); 
        fclose(file);
        char len[8];
        sprintf(len,"%d",nByte);
        strcat(headers,len);
        strcat(headers,"\r\n");
        strcat(headers,"\r\n");

        
        iov[0].iov_base = (void*)state_line;
        iov[0].iov_len = strlen(state_line);
        iov[1].iov_base = (void*)headers;
        iov[1].iov_len = strlen(headers);
        iov[2].iov_base = (void*)f;
        iov[2].iov_len = nByte;
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