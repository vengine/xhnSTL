#ifndef __VEngine__xhn_thread_map__
#define __VEngine__xhn_thread_map__

#ifdef __cplusplus

#include "xhn_thread.hpp"
#include "xhn_lock.hpp"
#include "xhn_hash_map.hpp"

namespace xhn {
    template <typename T>
    class thread_map : protected hash_map<T, thread_ptr>
    {
    public:
        thread_ptr get_thread(const T& key) {
            euint hash_value = this->m_hash_proc(key);
			euint ukey = hash_value & HASH_MASK;
            RWLock::Instance inst = hash_map<T, thread_ptr>::m_buckets[ukey].get_write_lock();
            thread_ptr* tmp = hash_map<T, thread_ptr>::m_buckets[ukey].find_unlock(key);
            if (tmp) {
                return *tmp;
            }
            else {
                thread_ptr ret = VNEW thread;
                while (!ret->is_running()) {}
                hash_map<T, thread_ptr>::m_buckets[ukey].insert_unlock(key, ret);
                return ret;
            }
        }
        void init_thread(const T& key) {
            euint hash_value = this->m_hash_proc(key);
			euint ukey = hash_value & HASH_MASK;
            RWLock::Instance inst = hash_map<T, thread_ptr>::m_buckets[ukey].get_write_lock();
            thread_ptr* tmp = hash_map<T, thread_ptr>::m_buckets[ukey].find_unlock(key);
            if (!tmp) {
                thread_ptr ret = VNEW thread;
                while (!ret->is_running()) {}
                hash_map<T, thread_ptr>::m_buckets[ukey].insert_unlock(key, ret);
            }
        }
    };
}

#endif

#endif /* defined(__VEngine__xhn_thread_map__) */
