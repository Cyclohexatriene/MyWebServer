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

void http::response(char* filepath){
    FILE* file = fopen(filepath,"r");
    char f[131072]; // number means max file size. 131072 Byte = 128 KB.
    if(file == nullptr){
        strcpy(filepath,"./root/404notfound.html");
        file = fopen(filepath,"r");
    }
    printf("path = %s\n",filepath);
    char contentType[64];
    int idx = 0;
    for(int i = 0;i < strlen(filepath);i++){
        if(filepath[i] == '.') idx = 0;
        else{
            contentType[idx] = filepath[i];
            idx++;
        }
    }
    contentType[idx] = '\0';

    strcpy(state_line,"HTTP/1.1 200 OK\r\n");
    strcpy(headers,"Access-Control-Allow-Origin: *\r\nContent-Type: ");
    
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
    return;
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

        response(filepath);
        state = 1;
    }
    else{
        strncpy(request_type,package,4);
        request_type[4] = '\0';
        if(strcmp(request_type,"POST") == 0){
            char request_path[128];
            int idx = 5;
            while(package[idx]!=' '){
                request_path[idx-5] = package[idx];
                idx++;
            }
            request_path[idx-5] = '\0';
            
            smatch m;
            regex e("\r\n\r\n");
            regex_search(package_str, m, e);
            printf("The posted info is: %s\n",m.suffix().str().c_str());
            if(strcmp(request_path,"/dopost") == 0){
                string posted_info = m.suffix().str();
                string login, pass;
                int idx = 6; // Cutoff "login="
                while(posted_info[idx] != '&'){
                    login.push_back(posted_info[idx]);
                    idx++;
                }
                pass = posted_info.substr(idx+6); // Cutoff "&pass="
                cout << "login = " << login << ", pass = " << pass << endl;

                char filepath[128] = {0};
                if(login == "3200104203" && pass == "4203") strcat(filepath,"./root/login_successful.html");
                else strcat(filepath,"./root/login_failed.html");
                response(filepath);
            }
            else{
                char filepath[128] = "./root/inforecved.html";
                response(filepath);
            }
            state = 1;
        }
        else{
            state = 2;
        }
    }
}