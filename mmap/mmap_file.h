#ifndef MMAP_FILE_H
#define MMAP_FILE_H

#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

class mmap_file
{
private:
    int fd;
    void *start; /* The start of mmap. */
    struct stat st;
public:
    mmap_file();
    mmap_file(string path);
    ~mmap_file();
    void openFile(string path);
    void* getStart();
    int getSize();
};

/* ---------- Definitions ----------*/

mmap_file::mmap_file()
{
    fd = -1;
    start = NULL;
}

mmap_file::mmap_file(string path)
{
    fd = open(path.c_str(),O_CREAT | O_RDWR);
    fstat(fd,&st);
    start = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
}

void mmap_file::openFile(string path)
{
    if(start != NULL)
    {
        munmap(start,st.st_size);
        close(fd);
    }
    fd = open(path.c_str(),O_CREAT | O_RDWR);
    fstat(fd,&st);
    start = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
}

mmap_file::~mmap_file()
{
    if(start != NULL)
    {
        munmap(start,st.st_size);
        close(fd);
    }
}

void* mmap_file::getStart()
{
    return start;
}

int mmap_file::getSize()
{
    return (int)st.st_size;
}

#endif