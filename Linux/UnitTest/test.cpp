#include <xhnSTL/common.h>
#include <xhnSTL/etypes.h>
#include <xhnSTL/xhn_lock.hpp>
#include <xhnSTL/xhn_thread.hpp>
int main(int argc, char *argv[])
{
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
