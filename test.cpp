#include <iostream>
#include <pthread.h>
#include "log/block_queue.h"
#include <unistd.h>
using namespace std;

block_queue<int> bq;

void* producer(void* argv)
{
    for(int i=0; i<10000; i++)
    {
        if(bq.push(i))
            printf("produced: %d\n", i);
        else i--;
    }
    return (void*)0;
}

void* consumer(void* argv)
{
    int a;
    for(int i=0; i<10000; i++)
    {
        if(bq.pop(a))
            printf("consumed: %d\n", a);
        else i--;
    }
    return (void*)0;
}

int main()
{
    pthread_t tidP;
    pthread_t tidC;
    pthread_create(&tidC,NULL,consumer,NULL);
    pthread_create(&tidP,NULL,producer,NULL);
    pthread_join(tidP,NULL);
    pthread_join(tidC,NULL);
    printf("Done.\n");
    return 0;
}