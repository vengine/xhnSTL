#include "xhn_thread.hpp"
#include "cpu.h"
#ifdef __APPLE__
#include <unistd.h>
#include <limits.h>

#include <mach/mach_init.h>
#include <mach/thread_policy.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

#elif defined(LINUX)
#include <sys/errno.h>
#include <unistd.h>
#include <limits.h>
#endif

xhn::SpinLock xhn::thread::s_thread_stack_range_map_lock;
xhn::range_map<vptr, xhn::thread*> xhn::thread::s_thread_stack_range_map;

const xhn::static_string xhn::thread::lambda_task::StrLambdaTask("LambdaTask");
const xhn::static_string xhn::thread::lambda_task::type() const
{
    return StrLambdaTask;
}
bool xhn::thread::lambda_task::run()
{
    TaskStatus ret = m_lambda();
    return ret == Completed;
}
void xhn::thread::lambda_task::set_lambda(Lambda<TaskStatus ()>& lambda)
{
    m_lambda = lambda;
}

void xhn::thread::milli_sleep(euint32 millisecond)
{
#if defined(_WIN32) || defined(_WIN64)
    SleepEx(millisecond, FALSE);
#elif defined(__APPLE__)
    euint32 second = millisecond / 1000;
    millisecond = millisecond % 1000;
    timespec t = {(__darwin_time_t)second, (long)(millisecond * 1000 * 1000)};
    struct timespec remainder;
    if (nanosleep(&t, &remainder) == -1) {
        if (errno == EINTR) {
            (void)printf("nanosleep interrupted\n");
            (void)printf("Remaining secs: %ld\n", remainder.tv_sec);
            (void)printf("Remaining nsecs: %ld\n", remainder.tv_nsec);
        }
        else perror("nanosleep");
    }
#elif defined(LINUX)
    euint32 second = millisecond / 1000;
    millisecond = millisecond % 1000;
    timespec t = {(__time_t)second, (long)(millisecond * 1000 * 1000)};
    struct timespec remainder;
    if (nanosleep(&t, &remainder) == -1) {
        if (errno == EINTR) {
            (void)printf("nanosleep interrupted\n");
            (void)printf("Remaining secs: %ld\n", remainder.tv_sec);
            (void)printf("Remaining nsecs: %ld\n", remainder.tv_nsec);
        }
        else perror("nanosleep");
    }
#else
#error
#endif
}

void xhn::thread::micro_sleep(euint32 microsecond)
{
#if defined(_WIN32) || defined(_WIN64)
    SleepEx(microsecond, FALSE);
#elif defined(__APPLE__)
    euint32 second = microsecond / (1000 * 1000);
    microsecond = microsecond % (1000 * 1000);
    timespec t = {(__darwin_time_t)second, (long)(microsecond * 1000)};
    struct timespec remainder;
    if (nanosleep(&t, &remainder) == -1) {
        if (errno == EINTR) {
            (void)printf("nanosleep interrupted\n");
            (void)printf("Remaining secs: %ld\n", remainder.tv_sec);
            (void)printf("Remaining nsecs: %ld\n", remainder.tv_nsec);
        }
        else perror("nanosleep");
    }
#elif defined(LINUX)
    euint32 second = microsecond / (1000 * 1000);
    microsecond = microsecond % (1000 * 1000);
    timespec t = {(__time_t)second, (long)(microsecond * 1000)};
    struct timespec remainder;
    if (nanosleep(&t, &remainder) == -1) {
        if (errno == EINTR) {
            (void)printf("nanosleep interrupted\n");
            (void)printf("Remaining secs: %ld\n", remainder.tv_sec);
            (void)printf("Remaining nsecs: %ld\n", remainder.tv_nsec);
        }
        else perror("nanosleep");
    }
#else
#error
#endif
}

void xhn::thread::nano_sleep(euint32 nanosecond)
{
#if defined(_WIN32) || defined(_WIN64)
    SleepEx(1, FALSE);
#elif defined(__APPLE__)
    timespec t = {(__darwin_time_t)0, (long)nanosecond};
    struct timespec remainder;
    if (nanosleep(&t, &remainder) == -1) {
        if (errno == EINTR) {
            (void)printf("nanosleep interrupted\n");
            (void)printf("Remaining secs: %ld\n", remainder.tv_sec);
            (void)printf("Remaining nsecs: %ld\n", remainder.tv_nsec);
        }
        else perror("nanosleep");
    }
#elif defined(LINUX)
    timespec t = {(__time_t)0, (long)nanosecond};
    struct timespec remainder;
    if (nanosleep(&t, &remainder) == -1) {
        if (errno == EINTR) {
            (void)printf("nanosleep interrupted\n");
            (void)printf("Remaining secs: %ld\n", remainder.tv_sec);
            (void)printf("Remaining nsecs: %ld\n", remainder.tv_nsec);
        }
        else perror("nanosleep");
    }
#else
#error
#endif
}

void* xhn::thread::thread_proc(void* self)
{
    int oldState;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);
    thread* t = (thread*)self;
	t->run();
	return NULL;
}
void xhn::thread::run()
{
    AtomicCompareExchange(1, 0, &m_is_errored);
begin:
    AtomicCompareExchange(1, 0, &m_is_finished);
    AtomicCompareExchange(1, 0, &m_is_stopped);
    {
        pthread_mutex_lock(&m_mutex);
        m_tasks.clear();
        AtomicCompareExchange(0, 1, &m_is_completed);
        pthread_mutex_unlock(&m_mutex);
    }
    try {
        AtomicCompareExchange(0, 1, &m_is_running);
        while (1) {
            if (AtomicLoad32(&m_is_finished))
                break;
            {
                pthread_mutex_lock(&m_mutex);
                while (m_tasks.empty() && !AtomicLoad32(&m_is_finished)) {
                    AtomicCompareExchange(0, 1, &m_is_completed);
                    pthread_cond_wait(&m_cond, &m_mutex);
                }
                pthread_mutex_unlock(&m_mutex);
            }
            if (AtomicLoad32(&m_is_finished))
                break;
            task_ptr t;
            {
                pthread_mutex_lock(&m_mutex);
                if (!m_tasks.empty()) {
                    t = m_tasks.front();
                    m_tasks.pop_front();
                }
                else {
                    AtomicCompareExchange(0, 1, &m_is_completed);
                }
                pthread_mutex_unlock(&m_mutex);
            }
            if (t) {
#if DEBUG_THREAD
                const static_string task_type = t->type();
                printf("@@begin task %s\n", task_type.c_str());
#endif
                /// 返回真表示执行结束
                if (t->run()) {
                    pthread_mutex_lock(&m_mutex);
                    if (m_tasks.empty()) {
                        AtomicCompareExchange(0, 1, &m_is_completed);
                    }
                    pthread_mutex_unlock(&m_mutex);
                }
                else {
                    pthread_mutex_lock(&m_mutex);
                    /// 必须要push到队列前
                    m_tasks.push_front(t);
                    pthread_mutex_unlock(&m_mutex);
                }
#if DEBUG_THREAD
                printf("@@end task %s\n", task_type.c_str());
#endif
            }
        }
    }
    catch(Exception& e) {
        const char* msg = e.what();
        printf("thread exit %s\n", msg);
        AtomicCompareExchange(1, 0, &m_is_running);
        AtomicCompareExchange(0, 1, &m_is_errored);
        goto begin;
    }
    AtomicCompareExchange(0, 1, &m_is_stopped);
}
void xhn::thread::add_task(task_ptr t)
{
    pthread_mutex_lock(&m_mutex);
    m_tasks.push_back(t);
    AtomicCompareExchange(1, 0, &m_is_completed);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}
void xhn::thread::add_lambda_task(Lambda<TaskStatus ()>& lambda)
{
    lambda_task* task = VNEW lambda_task;
    task->set_lambda(lambda);
    add_task(task);
}
void xhn::thread::stop() {
    /// 需要发送消息唤醒线程
    pthread_mutex_lock(&m_mutex);
    AtomicCompareExchange(0, 1, &m_is_finished);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}
void xhn::thread::reset() {
    AtomicCompareExchange(1, 0, &m_is_errored);
}
void xhn::thread::force_stop()
{
    while (!is_stopped()) {
        stop();
    }
}
void xhn::thread::force_restart()
{
    /// 如果没执行完，则强制清空tasks，并while等待，知道执行完为止
    while (!AtomicLoad32(&m_is_completed)) {
        pthread_mutex_lock(&m_mutex);
        m_tasks.clear();
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }
    reset();
}
void xhn::thread::wait_completed()
{
    while (!AtomicLoad32(&m_is_completed)) {
        pthread_yield_np();
    }
}
void xhn::thread::join()
{
    pthread_join(m_pt, NULL);
}

void xhn::thread::clear()
{
    pthread_mutex_lock(&m_mutex);
    m_tasks.clear();
    AtomicCompareExchange(0, 1, &m_is_completed);
    pthread_mutex_unlock(&m_mutex);
}

bool xhn::thread::I_am_this_thread()
{
    int i;
    return m_stack_range.is_in_range(&i);
}

bool xhn::thread::has_tasks(const set<static_string>& task_types)
{
    pthread_mutex_lock(&m_mutex);
    list<task_ptr, ThreadAllocator>::iterator iter = m_tasks.begin();
    list<task_ptr, ThreadAllocator>::iterator end = m_tasks.end();
    for (; iter != end; iter++) {
        task_ptr t = *iter;
        const static_string type = t->type();
        set<static_string>::const_iterator ci = task_types.find(type);
        set<static_string>::const_iterator ce = task_types.end();
        if (ci == ce) {
            pthread_mutex_unlock(&m_mutex);
            return true;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return false;
}

void xhn::thread::stop_thread_and_join(SmartPtr<thread>& t)
{
    if (t) {
        while (!t->is_stopped()) {
            t->stop();
        }
        t->join();
        t = NULL;
    }
}

void xhn::thread::move_to_realtime_scheduling_class()
{
#ifdef __APPLE__
    mach_timebase_info_data_t timebase_info;
    mach_timebase_info(&timebase_info);
    
    const uint64_t NANOS_PER_MSEC = 1000000ULL;
    double clock2abs = ((double)timebase_info.denom / (double)timebase_info.numer) * NANOS_PER_MSEC;
    
    /*
     * THREAD_TIME_CONSTRAINT_POLICY:
     *
     * This scheduling mode is for threads which have real time
     * constraints on their execution.
     *
     * Parameters:
     *
     * period: This is the nominal amount of time between separate
     * processing arrivals, specified in absolute time units.  A
     * value of 0 indicates that there is no inherent periodicity in
     * the computation.
     *
     * computation: This is the nominal amount of computation
     * time needed during a separate processing arrival, specified
     * in absolute time units.
     *
     * constraint: This is the maximum amount of real time that
     * may elapse from the start of a separate processing arrival
     * to the end of computation for logically correct functioning,
     * specified in absolute time units.  Must be (>= computation).
     * Note that latency = (constraint - computation).
     *
     * preemptible: This indicates that the computation may be
     * interrupted, subject to the constraint specified above.
     */
    thread_time_constraint_policy_data_t policy;
    policy.period      = 0;
    policy.computation = (uint32_t)(1 * clock2abs); // 1 ms of work
    policy.constraint  = (uint32_t)(1 * clock2abs);
    policy.preemptible = FALSE;

    int kr = thread_policy_set(pthread_mach_thread_np(m_pt),
                               THREAD_TIME_CONSTRAINT_POLICY,
                               (thread_policy_t)&policy,
                               THREAD_TIME_CONSTRAINT_POLICY_COUNT);
    if (kr != KERN_SUCCESS) {
        mach_error("thread_policy_set:", kr);
        exit(1);
    }
#else
    /// 什么也不做
#endif
}

xhn::thread::thread()
: m_is_completed(0)
, m_is_finished(0)
, m_is_running(0)
, m_is_stopped(0)
, m_is_errored(0)
{
    pthread_attr_t thread_attr;
    size_t stack_size;
    int status;

    status = pthread_attr_init (&thread_attr);
    EAssert(!status, "Create attr");
    status = pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED);
    EAssert(!status, "Set detach");
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
    status = pthread_attr_getstacksize (&thread_attr, &stack_size);
    EAssert (!status, "Get stack size");
#if BIT_WIDTH == 32
    printf ("Default stack size is %u; minimum is %u\n",
            stack_size, PTHREAD_STACK_MIN);
#else
    printf ("Default stack size is %lu; minimum is %u\n",
            stack_size, PTHREAD_STACK_MIN);
#endif
    vptr base = (void *) malloc(1024 * 1024);
    status = pthread_attr_setstack(&thread_attr, base, 1024 * 1024);
    EAssert (!status, "Set stack");
    status = pthread_attr_getstacksize (&thread_attr, &stack_size);
    EAssert (!status, "Get stack size");
#if BIT_WIDTH == 32
    printf ("Default stack size is %u; minimum is %u\n",
            stack_size, PTHREAD_STACK_MIN);
#else
    printf ("Default stack size is %lu; minimum is %u\n",
            stack_size, PTHREAD_STACK_MIN);
#endif
    m_stack_range.m_begin_addr = base;
    m_stack_range.m_size = 1024 * 1024;
#endif
#if BIT_WIDTH == 32
    printf("%x\n", (ref_ptr)this);
#else
    printf("%llx\n", (ref_ptr)this);
#endif
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
    SpinLock::Instance inst = s_thread_stack_range_map_lock.Lock();
    ref_ptr end_addr = (ref_ptr)m_stack_range.m_begin_addr + m_stack_range.m_size;
    s_thread_stack_range_map.insert(m_stack_range.m_begin_addr, (vptr)end_addr, this);
    printf("begin create thread\n");
	pthread_create(&m_pt,
		&thread_attr,
		thread_proc,
		(void *) this);
    printf("end create thread\n");
}
xhn::thread::~thread()
{
    AtomicCompareExchange(0, 1, &m_is_finished);
    {
        /// 需要发送消息唤醒线程
        pthread_mutex_lock(&m_mutex);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }
	while (!AtomicCompareExchange(1, 1, &m_is_stopped)) {
        AtomicCompareExchange(0, 1, &m_is_finished);
        pthread_mutex_lock(&m_mutex);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }
    pthread_join(m_pt, NULL);
    SpinLock::Instance inst = s_thread_stack_range_map_lock.Lock();
    s_thread_stack_range_map.remove(m_stack_range.m_begin_addr);
    if (m_stack_range.m_begin_addr) {
        free(m_stack_range.m_begin_addr);
    }
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
}

void xhn::thread::assign_to_local_thread(thread_ptr& local_thread, thread_ptr& global_thread, SpinLock& lock)
{
    {
        xhn::SpinLock::Instance inst = lock.Lock();
        local_thread = global_thread;
    }
    if (!local_thread) {

        local_thread = VNEW xhn::thread;
        while (!local_thread->is_running()) {}

        {
            xhn::SpinLock::Instance inst = lock.Lock();
            if (!global_thread) {
                global_thread = local_thread;
            }
            else {
                local_thread = global_thread;
            }
        }
    }
}
