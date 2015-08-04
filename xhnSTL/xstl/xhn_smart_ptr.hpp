/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef SMART_PTR_H
#define SMART_PTR_H
#include "common.h"
#include "etypes.h"
#include "xhn_vector.hpp"
#include "xhn_atomic_operation.hpp"
#include "xhn_lock.hpp"
#include "xhn_memory.hpp"
namespace xhn
{
template <typename T,
typename INC_CALLBACK,
typename DEST_CALLBACK,
typename GARBAGE_COLLECTOR>
class SmartPtr;

template <typename T>
struct FDestCallbackProc
{
	inline void operator() (T* ptr) {}
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
template <typename T>
struct FIncCallbackProc
{
    void operator() (T* ptr) {
	}
};
    
    
template< typename T,
          typename INC_CALLBACK = FIncCallbackProc<T>,
          typename DEST_CALLBACK = FDestCallbackProc<T>,
          typename GARBAGE_COLLECTOR = FGarbageCollectProc<T> >
class WeakPtr : public WeakPtrBase
{
    friend class xhn::SmartPtr<T, INC_CALLBACK, DEST_CALLBACK, GARBAGE_COLLECTOR>;
    friend class WeakNodeList;
private:
    WeakPtr(const WeakPtr& ptr) {
        /// do nothing
    }
    const WeakPtr& operator = (const WeakPtr& ptr) {
        /// do nothing
    }
public:
#ifdef DEBUG
    inline void PrintDebugInfo() const {
        m_weakLock.PrintDebugInfo();
    }
    inline bool TestDebug() const {
        return m_weakLock.TestDebug();
    }
#endif
    WeakPtr() {}
    ~WeakPtr()
    {
        RefSpinLock::Instance inst = m_weakLock.Lock();
        if (m_nodeList && m_node) {
            /// 自己节点已经锁住了，所以用半锁
            m_nodeList->DeleteNode_HalfLocked(m_node);
        }
    }
    inline xhn::SmartPtr<T, INC_CALLBACK, DEST_CALLBACK, GARBAGE_COLLECTOR> ToStrongPtr() {
        xhn::SmartPtr<T, INC_CALLBACK, DEST_CALLBACK, GARBAGE_COLLECTOR> ret;
        RefSpinLock::Instance inst0 = m_weakLock.Lock();
        /// 有一种情况，此时的引用计数已经是0了，这时等待销毁资源中，但是另一个线程则尝试将弱指针转强
        if (m_node && m_node->m_ptr && m_node->m_ptr->ref_count) {
            bool has_not_strong_destory = false;
            {
                RefSpinLock::Instance inst = m_node->m_ptr->ref_lock.Lock();
                if (!m_node->m_ptr->strong_destory_flag) {
                    has_not_strong_destory = true;
                }
                AtomicIncrement(&m_node->m_ptr->weak_to_strong_flag);
            }
            if (has_not_strong_destory) {
                ret = (T*)m_node->m_ptr;
            }
            AtomicDecrement(&m_node->m_ptr->weak_to_strong_flag);
            return ret;
        }
        else {
            return ret;
        }
    }
};
    
/// 一个RefObject的派生类，直接VNEW出来以后它的ref_count是0，这是必须将该对象赋值给SmartPtr。这样保存起来
/// 如果该对象没有赋值给SmartPtr，而是作为参数传递给一个参数类型是SmartPtr的函数，那么，
/// 该类转为SmartPtr参数时ref_count为1，函数返回时ref_count为0，就直接销毁对象了
/// 这可能会引起一个bug
    
template< typename T,
          typename INC_CALLBACK = FIncCallbackProc<T>,
          typename DEST_CALLBACK = FDestCallbackProc<T>,
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
	T* m_ptr;
	DEST_CALLBACK m_dest_callback;
    INC_CALLBACK m_inc_callback;
public:
	inline void _inc(T* ptr)
	{
		if (ptr)
		{
			AtomicIncrement(&ptr->ref_count);
            m_inc_callback(ptr);
		}
	}
	inline void _dec(T* ptr)
	{
		if (ptr) {
			if (!AtomicDecrement(&ptr->ref_count)) {
                bool has_not_weak_to_strong = false;
                bool ref_count_is_zero = false;
                {
                    RefSpinLock::Instance inst = ptr->ref_lock.Lock();
                    if (!ptr->weak_to_strong_flag) {
                        has_not_weak_to_strong = true;
                    }
                    if (!ptr->ref_count) {
                        ref_count_is_zero = true;
                    }
                    AtomicIncrement(&ptr->strong_destory_flag);
                }
                if (ref_count_is_zero && has_not_weak_to_strong) {
                    m_dest_callback(ptr);
                    delete ptr;
                    return;
                }
                AtomicDecrement(&ptr->strong_destory_flag);
            }
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
		return m_ptr;
	}
	inline T* operator = (const SmartPtr ptr)
	{
		_inc(ptr.m_ptr);
		_dec(m_ptr);
		m_ptr = ptr.m_ptr;
		return m_ptr;
	}
	inline bool operator < (const SmartPtr& ptr) const
	{
		return m_ptr < ptr.m_ptr;
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
		return m_ptr;
	}
	inline const T* get() const
	{
		return m_ptr;
	}
	inline T* operator ->() {
		return m_ptr;
	}
	inline const T* operator ->() const {
		return m_ptr;
	}
	inline bool operator!() const {
		return !m_ptr;
	}
    inline operator bool () const {
        return m_ptr != NULL;
    }

	CheckoutHandle Checkout()
	{
		CheckoutHandle ret(m_ptr->clone(), m_ptr);
		return ret;
	}

	bool Submit(CheckoutHandle& src, GARBAGE_COLLECTOR& gc)
	{
#ifdef _WIN32
		if (InterlockedCompareExchange((LONG volatile *)&m_ptr, (LONG)src.m_cloned, (LONG)src.m_original) == (LONG)src.m_original) {
#else
        if (OSAtomicCompareAndSwapPtr((void*)src.m_original, (void*)src.m_cloned, (void* volatile *)&m_ptr)) {
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

    void ToWeakPtr(WeakPtr<T, INC_CALLBACK, DEST_CALLBACK, GARBAGE_COLLECTOR>& result) {
        if (m_ptr) {
            RefSpinLock::Instance inst0 = result.m_weakLock.Lock();
            if (result.m_node && result.m_nodeList) {
                result.m_nodeList->DeleteNode_HalfLocked(result.m_node);
            }
            RefSpinLock::Instance inst1 = m_ptr->weak_node_list.m_lock.Lock();
            WeakNode* node =
            m_ptr->weak_node_list.NewNode_NotLocked(m_ptr);
            result.m_node = node;
            result.m_nodeList = &m_ptr->weak_node_list;
            node->m_weakPtr = &result;
        }
    }
};
    
template< typename T>
class Handle
{
private:
    T* m_ptr;
public:
    inline void _inc(T* ptr)
    {
        if (ptr)
        {
            AtomicIncrement(&ptr->ref_count);
        }
    }
    inline void _dec(T* ptr)
    {
        if (ptr) {
            if (!AtomicDecrement(&ptr->ref_count)) {
                bool has_not_weak_to_strong = false;
                bool ref_count_is_zero = false;
                {
                    RefSpinLock::Instance inst = ptr->ref_lock.Lock();
                    if (!ptr->weak_to_strong_flag) {
                        has_not_weak_to_strong = true;
                    }
                    if (!ptr->ref_count) {
                        ref_count_is_zero = true;
                    }
                    AtomicIncrement(&ptr->strong_destory_flag);
                }
                if (ref_count_is_zero && has_not_weak_to_strong) {
                    delete ptr;
                    return;
                }
                AtomicDecrement(&ptr->strong_destory_flag);
            }
        }
    }
    Handle()
    : m_ptr(NULL)
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
        return m_ptr;
    }
    inline T* operator = (const Handle ptr)
    {
        m_ptr = ptr.m_ptr;
        return m_ptr;
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
        return m_ptr;
    }
    inline const T* get() const
    {
        return m_ptr;
    }
    inline T* operator ->() {
        return m_ptr;
    }
    inline const T* operator ->() const {
        return m_ptr;
    }
    inline bool operator!() const {
        return !m_ptr;
    }
    inline operator bool () const {
        return m_ptr != NULL;
    }
    inline void Retain() {
        _inc(m_ptr);
    }
    inline void Release() {
        _dec(m_ptr);
        m_ptr = nullptr;
    }
};
}

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
