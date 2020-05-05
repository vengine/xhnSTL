/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef SMART_PTR_HPP
#define SMART_PTR_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_vector.hpp"
#include "xhn_string.hpp"
#include "xhn_atomic_operation.hpp"
#include "xhn_lock.hpp"
#include "xhn_memory.hpp"
namespace xhn
{
template <typename T,
typename DELETE_PROC,
typename GARBAGE_COLLECTOR>
class SmartPtr;

template <typename T>
struct FDeleteProc
{
	inline void operator() (volatile T* ptr) {
        delete (T*)ptr;
    }
};

template <typename T>
struct FGarbageCollectProc
{
    vector<T*> garbagePool;
	void operator() (T* garbage) {
		garbagePool.push_back(garbage);
	}
	~FGarbageCollectProc() {
		for (euint i = 0; i < garbagePool.size(); i++) {
			delete garbagePool[i];
		}
	}
};


template< typename T,
          typename DELETE_PROC = FDeleteProc<T>,
          typename GARBAGE_COLLECTOR = FGarbageCollectProc<T> >
class WeakPtr
{
    friend class xhn::SmartPtr<T, DELETE_PROC, GARBAGE_COLLECTOR>;
    friend class WeakNodeList;
private:
	RefSpinLock m_count_lock;
    mutable WeakCounter* m_weak_count;
private:
    void Dest() {
		volatile bool must_delete_count = false;
		RefSpinLock::Instance count_inst = m_count_lock.Lock();
        if (m_weak_count) {
            RefSpinLock::Instance content_inst = m_weak_count->content_lock.Lock();
            if (!AtomicDecrement(&m_weak_count->weak_count)) {
                if (!m_weak_count->ref_object) {
					must_delete_count = true;
                }
            }
        }
		if (must_delete_count) {
			delete m_weak_count;
		}
        m_weak_count = nullptr;
    }
public:
    WeakPtr(const WeakPtr& ptr) {
		RefSpinLock::Instance ptr_count_inst = ptr.m_count_lock.Lock();
        if (ptr.m_weak_count) {
            RefSpinLock::Instance content_inst = ptr.m_weak_count->content_lock.Lock();
            AtomicIncrement(&ptr.m_weak_count->weak_count);
			m_weak_count = ptr.m_weak_count;
        }
        else {
            m_weak_count = nullptr;
        }
    }
	WeakPtr()
		: m_weak_count(nullptr)
	{
	}
	~WeakPtr()
	{
		Dest();
	}
public:
    const WeakPtr& operator = (const WeakPtr& ptr) {
		RefSpinLock::Instance ptr_count_inst = ptr.m_count_lock.Lock();
        if (ptr.m_weak_count) {
            AtomicIncrement(&ptr.m_weak_count->weak_count);
        }
        Dest();
		{
			RefSpinLock::Instance count_inst = m_count_lock.Lock();
			m_weak_count = ptr.m_weak_count;
		}
		return *this;
    }
    inline xhn::SmartPtr<T, DELETE_PROC, GARBAGE_COLLECTOR> ToStrongPtr() const {
        xhn::SmartPtr<T, DELETE_PROC, GARBAGE_COLLECTOR> ret;
		RefSpinLock::Instance count_inst = m_count_lock.Lock();
        if (m_weak_count) {
            RefSpinLock::Instance content_inst = m_weak_count->content_lock.Lock();
            ret = (T*)m_weak_count->ref_object;
        }
        return ret;
    }
};

/// 一个RefObject的派生类，直接VNEW出来以后它的ref_count是0，这是必须将该对象赋值给SmartPtr。这样保存起来
/// 如果该对象没有赋值给SmartPtr，而是作为参数传递给一个参数类型是SmartPtr的函数，那么，
/// 该类转为SmartPtr参数时ref_count为1，函数返回时ref_count为0，就直接销毁对象了
/// 这可能会引起一个bug

template< typename T,
          typename DELETE_PROC = FDeleteProc<T>,
          typename GARBAGE_COLLECTOR = FGarbageCollectProc<T> >
class SmartPtr
{
public:
	class CheckoutHandle
	{
		friend class SmartPtr;
	private:
		T* m_cloned;
		T* m_original;
	private:
		CheckoutHandle(T* c, T* o)
			: m_cloned(c)
			, m_original(o)
		{
		}
	public:
        CheckoutHandle()
			: m_cloned(NULL)
			, m_original(NULL)
		{}
        CheckoutHandle(const CheckoutHandle& ch)
			: m_cloned(ch.m_cloned)
		    , m_original(ch.m_original)
		{
		}
		~CheckoutHandle()
		{
		}
		void operator= (const CheckoutHandle& ch)
		{
			m_cloned = ch.m_cloned;
			m_original = ch.m_original;
		}
		inline T* Get() {
			return m_cloned;
		}
	};
private:
	volatile T* m_ptr;
	DELETE_PROC m_delete_proc;
#if DEBUG_REFOBJECT
public:
    string stackframe;
#endif
public:
	inline void _inc(volatile T* ptr)
	{
		if (ptr)
		{
			AtomicIncrement(&ptr->ref_count);
#if DEBUG_REFOBJECT
            if (ptr->inc_callback) {
                ptr->inc_callback((RefObjectBase*)ptr, this);
            }
#endif
		}
	}
	inline void _dec(volatile T* ptr)
	{
		if (ptr) {
			if (!AtomicDecrement(&ptr->ref_count)) {
#if DEBUG_REFOBJECT
                if (ptr->dec_callback) {
                    ptr->dec_callback((RefObjectBase*)ptr, this);
                }
#endif
                m_delete_proc(ptr);
            }
#if DEBUG_REFOBJECT
            else {
                if (ptr->dec_callback) {
                    ptr->dec_callback((RefObjectBase*)ptr, this);
                }
            }
#endif
		}
	}
	SmartPtr()
		: m_ptr(NULL)
	{}
	SmartPtr(const SmartPtr& ptr)
	{
		_inc(ptr.m_ptr);
		m_ptr = ptr.m_ptr;
	}
	SmartPtr(T* ptr)
	{
		_inc(ptr);
		m_ptr = ptr;
	}
	~SmartPtr()
	{
		_dec(m_ptr);
		m_ptr = NULL;
	}
	inline T* operator = (T* ptr)
	{
		_inc(ptr);
		_dec(m_ptr);
		m_ptr = ptr;
		return (T*)m_ptr;
	}
	inline T* operator = (const SmartPtr ptr)
	{
		_inc(ptr.m_ptr);
		_dec(m_ptr);
		m_ptr = ptr.m_ptr;
		return (T*)m_ptr;
	}
	inline bool operator < (const SmartPtr& ptr) const
	{
		return m_ptr < ptr.m_ptr;
	}
    inline bool operator > (const SmartPtr& ptr) const
    {
        return m_ptr > ptr.m_ptr;
    }
	inline bool operator == (const SmartPtr& ptr) const
	{
		return m_ptr == ptr.m_ptr;
	}
	inline bool operator != (const SmartPtr& ptr) const
	{
		return m_ptr != ptr.m_ptr;
	}
	inline T* get()
	{
		return (T*)m_ptr;
	}
	inline const T* get() const
	{
		return (T*)m_ptr;
	}
	inline T* operator ->() {
		return (T*)m_ptr;
	}
	inline const T* operator ->() const {
		return (const T*)m_ptr;
	}
    inline T& operator *() {
        return (T&)*m_ptr;
    }
    inline const T& operator *() const {
        return (const T&)*m_ptr;
    }
	inline bool operator!() const {
		return !m_ptr;
	}
    inline operator bool () const {
        return m_ptr != NULL;
    }
    inline operator const T* () const {
        return (const T*)m_ptr;
    }
    inline operator T* () {
        return (T*)m_ptr;
    }
    inline euint32 hash_value() {
        return *((euint32*)(&m_ptr));
    }

	CheckoutHandle Checkout()
	{
		CheckoutHandle ret( ((T*)m_ptr)->clone(), (T*)m_ptr );
		return ret;
	}

	bool Submit(CheckoutHandle& src, GARBAGE_COLLECTOR& gc)
	{
#ifdef _WIN32
		if (InterlockedCompareExchange((LONG volatile *)&m_ptr, (LONG)src.m_cloned, (LONG)src.m_original) == (LONG)src.m_original) {
#elif defined(LINUX) && defined(AO_ATOMIC_OPS_H)
        if (AO_compare_and_swap((AO_t *)&m_ptr, (AO_t)src.m_original, (AO_t)src.m_cloned)) {
#else
        ///if (OSAtomicCompareAndSwapPtr((void*)src.m_original, (void*)src.m_cloned, (void* volatile *)&m_ptr)) {
        if (AtomicCompareExchangePtr((void*)src.m_original, (void*)src.m_cloned, (void* volatile *)&m_ptr)) {
#endif
			if (src.m_cloned) {
				AtomicIncrement(&src.m_cloned->ref_count);
			}
			if (src.m_original) {
				AtomicDecrement(&src.m_cloned->ref_count);
				if (!src.m_original->ref_count) {
					gc(src.m_original);
					src.m_original = NULL;
				}
			}
			return true;
		}
		else {
			if (src.m_cloned) {
				gc(src.m_cloned);
				src.m_cloned = NULL;
			}
			return false;
		}
	}

    void ToWeakPtr(WeakPtr<T, DELETE_PROC, GARBAGE_COLLECTOR>& result) {
        if (m_ptr) {
            AtomicIncrement(&m_ptr->weak_count->weak_count);
            result.Dest();
			{
				RefSpinLock::Instance count_inst = result.m_count_lock.Lock();
				result.m_weak_count = m_ptr->weak_count;
			}
        }
        else {
            result.Dest();
        }
    }
};

/// Handle 这个东西的存在是为了解决Command在channel里传递的问题，Handle可以直接拷贝，不像SmartPtr
template< typename T>
class Handle
{
private:
    mutable volatile T* m_ptr;
public:
    inline void _incSelf() const
    {
        if (m_ptr)
        {
            AtomicIncrement(&m_ptr->ref_count);
        }
    }
    inline void _decSelf() const
    {
        if (m_ptr) {
            if (!AtomicDecrement(&m_ptr->ref_count)) {
                delete m_ptr;
            }
        }
    }
    Handle()
    : m_ptr(nullptr)
    {}
    Handle(const Handle& ptr)
    {
        m_ptr = ptr.m_ptr;
    }
    Handle(T* ptr)
    {
        m_ptr = ptr;
    }
    ~Handle()
    {
        m_ptr = NULL;
    }
    inline T* operator = (T* ptr)
    {
        m_ptr = ptr;
        return (T*)m_ptr;
    }
    inline T* operator = (const Handle ptr)
    {
        m_ptr = ptr.m_ptr;
        return (T*)m_ptr;
    }
    inline bool operator < (const Handle& ptr) const
    {
        return m_ptr < ptr.m_ptr;
    }
    inline bool operator == (const Handle& ptr) const
    {
        return m_ptr == ptr.m_ptr;
    }
    inline bool operator != (const Handle& ptr) const
    {
        return m_ptr != ptr.m_ptr;
    }
    inline T* get()
    {
        return (T*)m_ptr;
    }
    inline const T* get() const
    {
        return (T*)m_ptr;
    }
    inline T* operator ->() {
        return (T*)m_ptr;
    }
    inline const T* operator ->() const {
        return (const T*)m_ptr;
    }
    inline bool operator!() const {
        return !m_ptr;
    }
    inline operator bool () const {
        return m_ptr != NULL;
    }
    inline void Retain() const {
        _incSelf();
    }
    inline void Release() const {
        _decSelf();
        m_ptr = nullptr;
    }
    inline euint32 hash_value() {
        return (euint32)m_ptr;
    }
};

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
