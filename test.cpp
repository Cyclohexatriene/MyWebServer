#include <iostream>
#include <pthread.h>
#include "log/block_queue.h"
#include <unistd.h>
#include "log/my_time.h"
#include "log/log.h"
using namespace std;

int main()
{
    Log::get_instance() -> init(50000,500);

    for(int i=0;i<100000;i++)
        Log::get_instance() -> write_log(LOG_DEBUG,"write log test: " + to_string(i));
    sleep(3);
}  