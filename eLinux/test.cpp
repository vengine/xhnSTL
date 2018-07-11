#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "xhn_lock.hpp"
#include "xhn_thread.hpp"

int main(int argc, char *argv[])
{
    ///void* p = malloc(128);
    ///free(p);
    ///printf("test\n");
    xhn::thread_ptr t = VNEW xhn::thread();
    while(!t->is_running()) {}
    volatile bool completed = false;

    xhn::Lambda<xhn::thread::TaskStatus()> proc([&completed](){
        printf("hello world from other thread!\n");
        completed = true;
        return xhn::thread::Completed;
    });

    t->add_lambda_task(proc);
    while (!completed) {}
    return 0;
}
