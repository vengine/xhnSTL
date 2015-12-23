#include "xhn_operation_manager.hpp"

ImplementRootRTTI(xhn::operation_data);

xhn::operation::~operation()
{
}

void xhn::operation::complete()
{
    if (m_handle) {
        send_data(m_handle);
    }
}

float xhn::operation_handle::get_progress_rate()
{
    operation* op = (operation*)m_operation.get();
    return op->get_progress_rate();
}

void xhn::operation_handle::cancel()
{
    operation* op = (operation*)m_operation.get();
    op->cancel();
}

xhn::operation_manager* xhn::operation_manager::s_operation_manager = nullptr;
xhn::operation_manager::operation_manager()
{
    for (int i = 0; i < 4; i++) {
        thread* t = VNEW thread;
        m_thread_pool.push_back(t);
        while (!t->is_running()) {}
    }
}
xhn::operation_manager::~operation_manager()
{}
xhn::operation_manager* xhn::operation_manager::get()
{
    if (!s_operation_manager) {
        s_operation_manager = VNEW operation_manager();
    }
    return s_operation_manager;
}
void xhn::operation_manager::add_operatoion(operation* op)
{
    euint min_num_tasks = MAX_EUINT;
    thread_ptr min_num_tasks_thread;
    vector<thread_ptr>::iterator iter = m_thread_pool.begin();
    vector<thread_ptr>::iterator end = m_thread_pool.end();
    for (; iter != end; iter++) {
        thread_ptr t = (*iter);
        if (!t->num_tasks()) {
            t->add_task(op);
            return;
        }
        else {
            if (t->num_tasks() < min_num_tasks) {
                min_num_tasks = t->num_tasks();
                min_num_tasks_thread = t;
            }
        }
    }
    min_num_tasks_thread->add_task(op);
}
