

#include "xhn_realtime_thread.hpp"
#include "xhn_thread.hpp"

void* xhn::realtime_thread::realtime_proc(void* thread)
{
    realtime_thread* t = reinterpret_cast<realtime_thread*>(thread);
#if defined(__APPLE__)
    mach_timebase_info_data_t timebase_info;
    mach_timebase_info(&timebase_info);
    
    const uint64_t NANOS_PER_MSEC = 1000000ULL;
    double clock2abs = ((double)timebase_info.denom / (double)timebase_info.numer) * NANOS_PER_MSEC;
    
    thread_time_constraint_policy_data_t policy;
    policy.period      = 0;
    policy.computation = (uint32_t)(1 * clock2abs); // 1 ms of work
    policy.constraint  = (uint32_t)(1 * clock2abs);
    policy.preemptible = FALSE;
    
    int kr = thread_policy_set(pthread_mach_thread_np(t->m_pt),
                               THREAD_TIME_CONSTRAINT_POLICY,
                               (thread_policy_t)&policy,
                               THREAD_TIME_CONSTRAINT_POLICY_COUNT);
    if (kr != KERN_SUCCESS) {
        mach_error("thread_policy_set:", kr);
        exit(1);
    }
#endif
    t->m_last_time_checkpoint = TimeCheckpoint::Tick();
    xhn::vector<realtime_task_ptr> exced_tasks;
    while (AtomicLoad32(&t->m_is_running)) {
        TimeCheckpoint currentTime = TimeCheckpoint::Tick();
        double elapsedTime =
        TimeCheckpoint::CalcElapsedTimeInNano(t->m_last_time_checkpoint, currentTime) / 1000000000.0;
        realtime_task_ptr task;
        {
            auto tasks = t->m_task_pool->GetTasks().Lock();
            if (tasks->size()) {
                task = tasks->back();
                tasks->pop_back();
            }
        }
        while (task)
        {
            task->run_once(elapsedTime);
            exced_tasks.push_back(task);
            auto tasks = t->m_task_pool->GetTasks().Lock();
            if (tasks->size()) {
                task = tasks->back();
                tasks->pop_back();
            } else {
                task = nullptr;
            }
        }
        {
            auto tasks = t->m_task_pool->GetTasks().Lock();
            for (auto& tk : exced_tasks) {
                tasks->push_back(tk);
            }
        }
        exced_tasks.clear();
        
        TimeCheckpoint afterExecutionTime = TimeCheckpoint::Tick();
        double executionTime =
        TimeCheckpoint::CalcElapsedTimeInNano(currentTime, afterExecutionTime) / 1000000000.0;
        t->m_last_time_checkpoint = currentTime;
        if (executionTime < t->m_time_interval) {
            double sleepTime = t->m_time_interval - executionTime;
            double sleepTimeInNanotime = sleepTime * 1000000000.0;
            xhn::thread::nano_sleep((euint32)sleepTimeInNanotime);
        }
    }
    AtomicStore32(1, &t->m_is_completed);
    return nullptr;
}

xhn::realtime_thread::~realtime_thread()
{
    AtomicStore32(0, &m_is_running);
    while (!AtomicLoad32(&m_is_completed)) {}
}

void xhn::realtime_thread::run()
{
    pthread_attr_t thread_attr;
    size_t stack_size;
    int status;
    
    status = pthread_attr_init (&thread_attr);
    EDebugAssert(!status, "Create attr");
    status = pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED);
    EDebugAssert(!status, "Set detach");
#if defined(ANDROID) || defined(__ANDROID__)
    sched_param SchedParam;
    int MidPriority,MaxPriority,MinPriority;
    MinPriority = sched_get_priority_max(SCHED_RR);
    MaxPriority = sched_get_priority_min(SCHED_RR);
    MidPriority = (MaxPriority + MinPriority)/2;
    SchedParam.sched_priority = MidPriority;
    pthread_attr_setschedparam(&thread_attr,&SchedParam);
    pthread_attr_setschedpolicy(&thread_attr,SCHED_RR);
#endif
    
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
    status = pthread_attr_getstacksize (&thread_attr, &stack_size);
    EDebugAssert (!status, "Get stack size");
#   if DEBUG_XHN_THREAD
#       if BIT_WIDTH == 32
    printf ("Default stack size is %u; minimum is %u\n",
            (euint)stack_size, PTHREAD_STACK_MIN);
#       else
    printf ("Default stack size is %lu; minimum is %u\n",
            stack_size, PTHREAD_STACK_MIN);
#       endif
#   endif
    vptr base = (void *) malloc(1024 * 1024);
#if defined(_WIN32) || defined(_WIN64)
	pthread_attr_setstacksize(&thread_attr, 1024 * 1024);
	status = pthread_attr_setstackaddr(&thread_attr, base);
#else
    status = pthread_attr_setstack(&thread_attr, base, 1024 * 1024);
#endif
    EDebugAssert (!status, "Set stack");
    status = pthread_attr_getstacksize (&thread_attr, &stack_size);
    EDebugAssert (!status, "Get stack size");
#   if DEBUG_XHN_THREAD
#       if BIT_WIDTH == 32
    printf ("Default stack size is %u; minimum is %u\n",
            (euint)stack_size, PTHREAD_STACK_MIN);
#       else
    printf ("Default stack size is %lu; minimum is %u\n",
            stack_size, PTHREAD_STACK_MIN);
#       endif
#   endif
#endif
    
#if DEBUG_XHN_THREAD
#   if BIT_WIDTH == 32
    printf("%x\n", (ref_ptr)this);
#   else
    printf("%llx\n", (ref_ptr)this);
#   endif
#endif
    
    AtomicStore32(1, &m_is_running);

#if DEBUG_XHN_THREAD
    printf("begin create thread\n");
#endif
    pthread_create(&m_pt,
                   &thread_attr,
                   realtime_proc,
                   (void *) this);
#if DEBUG_XHN_THREAD
    printf("end create thread\n");
#endif
}
