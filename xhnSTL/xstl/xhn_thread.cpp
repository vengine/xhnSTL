#include "xhn_thread.hpp"
#ifdef __APPLE__
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
    timespec t = {second, millisecond * 1000 * 1000};
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
    timespec t = {second, microsecond * 1000};
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
    timespec t = {0, nanosecond};
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
    m_is_errored = false;
begin:
    m_is_finished = false;
    m_is_stopped = false;
    {
        SpinLock::Instance taskInst = m_taskLock.Lock();
        m_tasks.clear();
        m_is_completed = true;
    }
    try {
        m_is_running = true;
        while (1) {
            if (m_is_finished)
                break;
            {
                ///struct timespec outtime;
                volatile bool isTaskEmpty = false;
                {
                    SpinLock::Instance taskInst = m_taskLock.Lock();
                    isTaskEmpty = m_tasks.empty();
                }
                ///while (isTaskEmpty && !m_is_finished) {
                if (isTaskEmpty && !m_is_finished) {
                    /// 必须是空的才会进这个循环，因此一开始就将m_is_completed置为true
                    m_is_completed = true;
                    
                    pthread_mutex_lock(&m_mutex);
                    ///outtime.tv_sec = 0;
                    ///outtime.tv_nsec = 1000 * 1000 * 100;
                    ///pthread_cond_timedwait_relative_np(&m_cond, &m_mutex, &outtime);
                    pthread_cond_wait(&m_cond, &m_mutex);
                    pthread_mutex_unlock(&m_mutex);
                    /**
                    {
                        SpinLock::Instance taskInst = m_taskLock.Lock();
                        isTaskEmpty = m_tasks.empty();
                    }
                     **/
                }
            }
            if (m_is_finished)
                break;
            task_ptr t;
            {
                SpinLock::Instance taskInst = m_taskLock.Lock();
                if (m_tasks.size()) {
                    t = m_tasks.front();
                    m_tasks.pop_front();
                }
                else {
                    m_is_completed = true;
                }
            }
            ///if (t.get()) {
            if (t) {
#if THREAD_DEBUG
                const static_string task_type = t->type();
                printf("@@begin task %s\n", task_type.c_str());
#endif
                /// 返回真表示执行结束
                if (t->run()) {
                    SpinLock::Instance taskInst = m_taskLock.Lock();
                    if (m_tasks.empty()) {
                        m_is_completed = true;
                    }
                }
                else {
                    SpinLock::Instance taskInst = m_taskLock.Lock();
                    /// 必须要push到队列前
                    m_tasks.push_front(t);
                }
#if THREAD_DEBUG
                printf("@@end task %s\n", task_type.c_str());
#endif
            }
        }
    }
    catch(Exception& e) {
        const char* msg = e.what();
        printf("thread exit %s\n", msg);
        m_is_running = false;
        m_is_errored = true;
        goto begin;
    }
    m_is_stopped = true;
}
void xhn::thread::add_task(task_ptr t)
{
    if (!m_is_stopped)
    {
        {
            SpinLock::Instance taskInst = m_taskLock.Lock();
            m_tasks.push_back(t);
            m_is_completed = false;
        }
        {
            pthread_mutex_lock(&m_mutex);
            pthread_cond_signal(&m_cond);
            pthread_mutex_unlock(&m_mutex);
        }
    }
}
void xhn::thread::add_lambda_task(Lambda<TaskStatus ()>& lambda)
{
    lambda_task* task = VNEW lambda_task;
    task->set_lambda(lambda);
    add_task(task);
}
void xhn::thread::stop() {
    m_is_finished = true;
    {
        /// 需要发送消息唤醒线程
        pthread_mutex_lock(&m_mutex);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }
}
void xhn::thread::reset() {
    m_is_errored = false;
}
void xhn::thread::force_stop()
{
    while (!is_stopped()) {
        stop();
    }
}
void xhn::thread::force_restart()
{
    {
        SpinLock::Instance taskInst = m_taskLock.Lock();
        m_tasks.clear();
    }
    while (!m_is_completed) {
        {
            SpinLock::Instance taskInst = m_taskLock.Lock();
            m_tasks.clear();
        }
        {
            pthread_mutex_lock(&m_mutex);
            pthread_cond_signal(&m_cond);
            pthread_mutex_unlock(&m_mutex);
        }
    }
    reset();
}
void xhn::thread::wait_completed()
{
    while (!m_is_completed) {}
}
void xhn::thread::join()
{
    pthread_join(m_pt, NULL);
}

void xhn::thread::clear()
{
    SpinLock::Instance taskInst = m_taskLock.Lock();
    m_tasks.clear();
    m_is_completed = true;
}

bool xhn::thread::I_am_this_thread()
{
    int i;
    return m_stack_range.is_in_range(&i);
}

bool xhn::thread::has_tasks(const set<static_string>& task_types)
{
    SpinLock::Instance taskInst = m_taskLock.Lock();
    list<task_ptr, ThreadAllocator>::iterator iter = m_tasks.begin();
    list<task_ptr, ThreadAllocator>::iterator end = m_tasks.end();
    for (; iter != end; iter++) {
        task_ptr t = *iter;
        const static_string type = t->type();
        set<static_string>::const_iterator ci = task_types.find(type);
        set<static_string>::const_iterator ce = task_types.end();
        if (ci == ce)
            return true;
    }
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

xhn::thread::thread()
: m_is_running(false)
, m_is_finished(false)
, m_is_stopped(false)
, m_is_completed(false)
, m_is_errored(false)
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
    printf ("Default stack size is %ld; minimum is %u\n",
            stack_size, PTHREAD_STACK_MIN);
    vptr base = (void *) malloc(1024 * 1024);
    status = pthread_attr_setstack(&thread_attr, base, 1024 * 1024);
    EAssert (!status, "Set stack");
    status = pthread_attr_getstacksize (&thread_attr, &stack_size);
    EAssert (!status, "Get stack size");
    printf ("Default stack size is %ld; minimum is %u\n",
            stack_size, PTHREAD_STACK_MIN);
    m_stack_range.m_begin_addr = base;
    m_stack_range.m_size = 1024 * 1024;
#endif
    printf("%llx\n", (ref_ptr)this);
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
    m_is_finished = true;
    {
        /// 需要发送消息唤醒线程
        pthread_mutex_lock(&m_mutex);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }
	while (!m_is_stopped) {
        m_is_finished = true;
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