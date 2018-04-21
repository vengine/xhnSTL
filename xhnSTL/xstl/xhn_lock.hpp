/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_LOCK_HPP
#define XHN_LOCK_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_memory.hpp"
#include "spin_lock.h"
#include "cpu.h"
namespace xhn
{
    /// \brief MutexLock
    ///
    /// 互斥锁
class MutexLock : public RefObject
{
public:
    mutable pthread_mutex_t m_lock;
public:
	class Instance
	{
		friend class MutexLock;
	private:
		pthread_mutex_t* m_prototype;
		inline Instance(pthread_mutex_t* lock)
        : m_prototype(lock)
		{}
	public:
		inline Instance(const Instance& inst)
        : m_prototype(inst.m_prototype)
		{}
		inline ~Instance()
		{
			pthread_mutex_unlock(m_prototype);
		}
	};
public:
	inline MutexLock()
	{
		pthread_mutex_init(&m_lock, NULL);
	}
	inline ~MutexLock()
	{
		pthread_mutex_destroy(&m_lock);
	}
	inline Instance Lock() const
	{
		pthread_mutex_lock(&m_lock);
		return Instance(&m_lock);
	}
};
    
    /// \brief RecursiveMutexLock
    ///
    /// 递归互斥锁
class RecursiveMutexLock : public RefObject
{
public:
    mutable pthread_mutex_t m_lock;
public:
    class Instance
    {
        friend class RecursiveMutexLock;
    private:
        pthread_mutex_t* m_prototype;
        inline Instance(pthread_mutex_t* lock)
        : m_prototype(lock)
        {}
    public:
        inline Instance(const Instance& inst)
        : m_prototype(inst.m_prototype)
        {}
        inline ~Instance()
        {
            pthread_mutex_unlock(m_prototype);
        }
    };
public:
    inline RecursiveMutexLock()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_lock, &attr);
    }
    inline ~RecursiveMutexLock()
    {
        pthread_mutex_destroy(&m_lock);
    }
    inline Instance Lock() const
    {
        pthread_mutex_lock(&m_lock);
        return Instance(&m_lock);
    }
};

    /// \brief RWLock
    ///
    /// 读写锁

class RWLock : public RefObject
{
private:
	pthread_rwlock_t m_lock;
	vptr m_userdata;
public:
	class Instance
	{
		friend class RWLock;
	private:
		pthread_rwlock_t* m_prototype;
		inline Instance(pthread_rwlock_t* lock)
			: m_prototype(lock)
		{}
	public:
		inline Instance(const Instance& inst)
			: m_prototype(inst.m_prototype)
		{}
		inline ~Instance()
		{
			pthread_rwlock_unlock(m_prototype);
		}
	};
public:
	inline RWLock()
	{
		pthread_rwlock_init(&m_lock, NULL);
		m_userdata = NULL;
	}
	inline ~RWLock()
	{
		pthread_rwlock_destroy(&m_lock);
	}
	inline Instance GetWriteLock()
	{
		pthread_rwlock_wrlock(&m_lock);
		return Instance(&m_lock);
	}
	inline Instance GetReadLock()
	{
		pthread_rwlock_rdlock(&m_lock);
		return Instance(&m_lock);
	}
	inline vptr GetUserdata() const {
		return m_userdata;
	}
	inline void SetUserdata(vptr userdata) {
		m_userdata = userdata;
	}
};

    /// \brief RWLock2
    ///
    /// 另一种读写锁

class RWLock2 : public RefObject
{
private:
    pthread_rwlock_t m_lock;
public:
	class Instance
	{
		friend class RWLock2;
	private:
		pthread_rwlock_t* m_prototype;
		inline Instance(pthread_rwlock_t* lock)
			: m_prototype(lock)
		{}
	public:
        inline Instance(const Instance& inst)
			: m_prototype(inst.m_prototype)
		{}
		inline ~Instance()
		{
			pthread_rwlock_unlock(m_prototype);
		}
	};
public:
    inline RWLock2()
	{
		pthread_rwlock_init(&m_lock, NULL);
	}
	inline ~RWLock2()
	{
		pthread_rwlock_destroy(&m_lock);
	}
	inline Instance GetWriteLock()
	{
		pthread_rwlock_wrlock(&m_lock);
        return Instance(&m_lock);
	}
	inline Instance GetReadLock()
	{
		pthread_rwlock_rdlock(&m_lock);
		return Instance(&m_lock);
	}
};
    
class RWLock3 : public RefObject
{
private:
    volatile esint32 m_read_lock_count;
    volatile esint32 m_write_lock;
public:
    class Instance
    {
        friend class RWLock3;
    private:
        RWLock3* m_prototype;
        const bool m_is_read_lock;
        inline Instance(RWLock3* lock, bool is_read_lock)
        : m_prototype(lock)
        , m_is_read_lock(is_read_lock)
        {}
    public:
        inline Instance(const Instance& inst)
        : m_prototype(inst.m_prototype)
        , m_is_read_lock(inst.m_is_read_lock)
        {}
        inline ~Instance()
        {
            if (m_is_read_lock) {
                AtomicDecrement(&m_prototype->m_read_lock_count);
            }
            else {
                AtomicStore32(0, &m_prototype->m_write_lock);
            }
        }
    };
public:
    inline RWLock3()
    : m_read_lock_count(0)
    , m_write_lock(0)
    {}
    inline Instance GetReadLock()
    {
        euint pause_count = 100;
        while (!AtomicCompareExchange(0, 1, &m_write_lock)) {
            nanopause(pause_count);
            pause_count += 100;
            if (pause_count > 1000) {
                pthread_yield_np();
                pause_count = 100;
            }
        }
        AtomicIncrement(&m_read_lock_count);
        AtomicStore32(0, &m_write_lock);
        return Instance(this, true);
    }
    inline Instance GetWriteLock()
    {
        euint pause_count = 100;
        while (!AtomicCompareExchange(0, 1, &m_write_lock)) {
            nanopause(pause_count);
            pause_count += 100;
            if (pause_count > 1000) {
                pthread_yield_np();
                pause_count = 100;
            }
        }
        return Instance(this, false);
    }
};

    /// \brief SpinLock
    ///
    /// 自旋锁

class SpinLock : public RefObject
{
private:
	mutable ELock m_lock;
	vptr m_userdata;
public:
	class Instance
	{
		friend class SpinLock;
	private:
		ELock* m_prototype;
		inline Instance(ELock* lock)
			: m_prototype(lock)
		{}
	public:
		inline Instance(const Instance& inst)
			: m_prototype(inst.m_prototype)
		{}
		inline ~Instance()
		{
			ELock_unlock(m_prototype);
		}
	};
public:
	inline SpinLock()
#ifdef __APPLE__
#ifndef OSSPINLOCK_DEPRECATED
    : m_lock(0)
    , m_userdata(NULL)
#else
    : m_lock(OS_UNFAIR_LOCK_INIT)
    , m_userdata(NULL)
#endif
#else
    : m_lock(0)
    , m_userdata(NULL)
#endif
	{}
	inline Instance Lock() const
	{
        ELock_lock(&m_lock);
		return Instance(&m_lock);
	}
	inline vptr GetUserdata() const {
		return m_userdata;
	}
	inline void SetUserdata(vptr userdata) {
		m_userdata = userdata;
	}
    inline bool TryLock() {
        return ELock_try(&m_lock);
    }
};

    /// \brief SpinObject
    ///
    /// 自旋物体
template <typename T>
class SpinObject : public RefObject
{
private:
    mutable ELock m_lock;
    mutable T m_data;
public:
    class Instance
    {
        friend class SpinObject;
    private:
        ELock* m_prototype;
        T* m_data;
        inline Instance(ELock* lock,
                        T* data)
        : m_prototype(lock)
        , m_data(data)
        {}
    public:
        inline Instance(const Instance& inst)
        : m_prototype(inst.m_prototype)
        , m_data(inst.m_data)
        {}
        inline ~Instance()
        {
            ELock_unlock(m_prototype);
        }
        inline T* operator->() {
            return m_data;
        }
        inline const T* operator->() const {
            return m_data;
        }
        inline T& operator*() {
            return *m_data;
        }
        inline const T& operator*() const {
            return *m_data;
        }
    };
public:
    inline SpinObject()
#ifdef __APPLE__
#ifndef OSSPINLOCK_DEPRECATED
    : m_lock(0)
#else
    : m_lock(OS_UNFAIR_LOCK_INIT)
#endif
#else
    : m_lock(0)
#endif
    {}
    template <typename ...ARGS>
    SpinObject(ARGS... args)
    : m_data(args...)
#ifdef __APPLE__
#ifndef OSSPINLOCK_DEPRECATED
    , m_lock(0)
#else
    , m_lock(OS_UNFAIR_LOCK_INIT)
#endif
#else
    , m_lock(0)
#endif
    {
    }
    inline Instance Lock()
    {
        ELock_lock(&m_lock);
        return Instance(&m_lock, &m_data);
    }
    inline Instance Lock() const
    {
        ELock_lock(&m_lock);
        return Instance(&m_lock, &m_data);
    }
};

    /// \brief MutexObject
    ///
    /// 互斥物体

template<typename T>
class MutexObject : public RefObject
{
public:
    mutable pthread_mutex_t m_lock;
    mutable T m_data;
public:
    class Instance
    {
        friend class MutexObject;
    private:
        pthread_mutex_t* m_prototype;
        T* m_data;
        inline Instance(pthread_mutex_t* lock, T* data)
        : m_prototype(lock)
        , m_data(data)
        {}
    public:
        inline Instance(const Instance& inst)
        : m_prototype(inst.m_prototype)
        , m_data(inst.m_data)
        {}
        inline ~Instance()
        {
            pthread_mutex_unlock(m_prototype);
        }
        inline T* operator ->() {
            return m_data;
        }
        inline const T* operator->() const {
            return m_data;
        }
        inline T& operator*() {
            return *m_data;
        }
        inline const T& operator*() const {
            return *m_data;
        }
    };
public:
    inline MutexObject()
    {
        pthread_mutex_init(&m_lock, NULL);
    }
    template <typename ...ARGS>
    MutexObject(ARGS... args)
    : m_data(args...)
    {
        pthread_mutex_init(&m_lock, NULL);
    }
    inline ~MutexObject()
    {
        pthread_mutex_destroy(&m_lock);
    }
    inline Instance Lock()
    {
        pthread_mutex_lock(&m_lock);
        return Instance(&m_lock, &m_data);
    }
    inline Instance Lock() const
    {
        pthread_mutex_lock(&m_lock);
        return Instance(&m_lock, &m_data);
    }
};
    
template<typename T>
class RecursiveMutexObject : public RefObject
{
public:
    mutable pthread_mutex_t m_lock;
    T m_data;
public:
    class Instance
    {
        friend class RecursiveMutexObject;
    private:
        pthread_mutex_t* m_prototype;
        T* m_data;
        inline Instance(pthread_mutex_t* lock, T* data)
        : m_prototype(lock)
        , m_data(data)
        {}
    public:
        inline Instance(const Instance& inst)
        : m_prototype(inst.m_prototype)
        , m_data(inst.m_data)
        {}
        inline ~Instance()
        {
            pthread_mutex_unlock(m_prototype);
        }
        inline T* operator ->() {
            return m_data;
        }
        inline const T* operator->() const {
            return m_data;
        }
    };
public:
    inline RecursiveMutexObject()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_lock, &attr);
    }
    template <typename ...ARGS>
    RecursiveMutexObject(ARGS... args)
    : m_data(args...)
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_lock, &attr);
    }
    inline ~RecursiveMutexObject()
    {
        pthread_mutex_destroy(&m_lock);
    }
    inline Instance Lock()
    {
        pthread_mutex_lock(&m_lock);
        return Instance(&m_lock, &m_data);
    }
};
    
}

/// \brief AutoMutexLock
///
/// 自动互斥锁

class AutoMutexLock
{
public:
    pthread_mutex_t m_lock;
public:
    AutoMutexLock() {
        pthread_mutex_init(&m_lock, NULL);
    }
    ~AutoMutexLock() {
        pthread_mutex_destroy(&m_lock);
    }
};
#define MutexSingleton_Decl(T) \
    private: \
        static xhn::SmartPtr<T> s_singleton; \
    private: \
        AutoMutexLock m_singletonLock; \
    public: \
        class Instance \
        { \
            friend class T; \
        private: \
            pthread_mutex_t* m_lockPrototype; \
            T* m_singleton; \
            inline Instance(pthread_mutex_t* lock, \
                            T* singleton) \
            : m_lockPrototype(lock) \
            , m_singleton(singleton) \
            {} \
        public: \
            inline Instance(const Instance& inst) \
            : m_lockPrototype(inst.m_lockPrototype) \
            , m_singleton(inst.m_singleton) \
            {} \
            inline ~Instance() \
            { \
                pthread_mutex_unlock(m_lockPrototype); \
            } \
            inline T* operator ->() { \
                return m_singleton; \
            } \
        }; \
        static Instance Lock();

#define MutexSingleton_Impl(T) \
xhn::SmartPtr<T> T::s_singleton; \
T::Instance T::Lock() \
{ \
    pthread_mutex_lock(&s_singleton->m_singletonLock.m_lock); \
    return Instance(&s_singleton->m_singletonLock.m_lock, s_singleton.get()); \
}

#define LockedSingletonDecl(classname, data) \
class classname : public RefObject \
{ \
private: \
static xhn::SmartPtr<classname> s_singleton; \
private: \
pthread_mutex_t m_lock; \
data m_data; \
public: \
class Instance \
{ \
friend class classname; \
private: \
pthread_mutex_t* m_lockPrototype; \
classname* m_##classname; \
inline Instance(pthread_mutex_t* lock, \
classname* _##classname) \
: m_lockPrototype(lock) \
, m_##classname(_##classname) \
{} \
public: \
inline Instance(const Instance& inst) \
: m_lockPrototype(inst.m_lockPrototype) \
, m_##classname(inst.m_##classname) \
{} \
inline ~Instance() \
{ \
pthread_mutex_unlock(m_lockPrototype); \
} \
inline classname* operator ->() { \
return m_##classname; \
} \
inline classname* Get() { \
return m_##classname; \
} \
}; \
public: \
classname (data d) \
: m_data(d) \
{ \
pthread_mutex_init(&m_lock, NULL); \
s_singleton = this; \
} \
virtual ~classname () \
{ \
pthread_mutex_destroy(&m_lock); \
s_singleton = NULL; \
} \
public: \
static Instance Lock(); \
inline data GetData() { return m_data; } \
inline void SetData(data d) { m_data = d; } \
};

#define LockedSingletonImpl(classname) \
xhn::SmartPtr<classname> classname::s_singleton; \
classname::Instance classname::Lock() { \
pthread_mutex_lock(&s_singleton->m_lock); \
return Instance(&s_singleton->m_lock, s_singleton.get()); \
}

#endif

#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
