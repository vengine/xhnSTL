/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef EMEM_HPP
#define EMEM_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "eassert.h"
#include "elog.h"
#include "xhn_exception.hpp"
#include "xhn_atomic_operation.hpp"

class BannedAllocObject
{
public:
	//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize )
	{
        xhnSTLExce(xhn::InvalidMemoryAllocException, "this object cant new operator");
	}
	void operator delete( void *p)
	{
        xhnSTLExce(xhn::InvalidMemoryFreeException, "this object cant delete operator");
	}
	//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize, const char* file,int line )
	{
        xhnSTLExce(xhn::InvalidMemoryAllocException, "this object cant new operator");
	}

	void operator delete( void *p, const char* file,int line )
	{
        xhnSTLExce(xhn::InvalidMemoryAllocException, "this object cant delete operator");
	}
	//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize, void* ptr )
	{
		EAssert(((ref_ptr)ptr % 16) == 0, "new object error");
		return ptr;
	}

	void operator delete( void *p, void* ptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void* operator new[]( size_t nSize )
	{
        xhnSTLExce(xhn::InvalidMemoryAllocException, "this object cant new operator");
	}

	void operator delete[]( void* ptr, size_t nSize )
	{
        xhnSTLExce(xhn::InvalidMemoryAllocException, "this object cant delete operator");
	}
};

class MemObject
{
public:
    MemObject() {}
    ~MemObject() {}
//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize )
    {
        return NMalloc( nSize );
    }
	void operator delete( void *p)
	{
		Mfree(p);
	}
//////////////////////////////////////////////////////////////////////////
    void* operator new( size_t nSize, const char* file,int line )
    {
        return _Malloc( nSize, file, line );
    }

	void operator delete( void *p, const char* file,int line )
    {
        Mfree(p);
    }
//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize, void* ptr )
	{
		EAssert(((ref_ptr)ptr % 16) == 0, "new object error");
		return ptr;
	}

	void operator delete( void *p, void* ptr )
	{
	}
//////////////////////////////////////////////////////////////////////////
	void* operator new[]( size_t nSize )
	{
		return NMalloc( nSize );
	}

	void operator delete[]( void* ptr, size_t nSize )
	{
		Mfree(ptr);
	}
};


class RefSpinLock
{
private:
	ELock m_lock;
public:
#ifdef DEBUG
    inline void PrintDebugInfo() const {
        printf("%d\n", m_lock);
    }
    inline bool TestDebug() const {
        return m_lock == 0;
    }
#endif
	class Instance
	{
		friend class RefSpinLock;
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
	inline RefSpinLock()
    : m_lock(0)
	{}
	inline Instance Lock()
	{
        ELock_lock(&m_lock);
		return Instance(&m_lock);
	}
    inline bool TryLock() const {
        return m_lock;
    }
};

/// WeakPtrBase m_weakPtr <-----
///    |                        |
///    |-------------------> WeakNode m_node <--
///    |                                        |
///    |                                         ------------------------
///    |                                                                 |
///    --------------------> WeakNodeList m_nodeList ---> WeakNode -> WeakNode -> WeakNode
///
/// WeakNode
///    |
///    |----------> WeakPtrBase m_weakPtr
///    |
///    -----------> RefObject m_ptr
///
/// WeakPtr : public WeakPtrBase

class RefObject;
class WeakNode;
class WeakNodeList;
class WeakPtrBase
{
    friend class WeakNodeList;
protected:
    RefSpinLock m_weakLock;
    WeakNodeList* m_nodeList;
    WeakNode* m_node;
protected:
    WeakPtrBase()
    : m_nodeList(NULL)
    , m_node(NULL)
    {}
};
class WeakNode : public MemObject
{
public:
    WeakPtrBase* m_weakPtr;
    WeakNode* m_next;
    WeakNode* m_prev;
    RefObject* m_ptr;
public:
    WeakNode(RefObject* ptr)
    : m_weakPtr(NULL)
    , m_next(NULL)
    , m_prev(NULL)
    , m_ptr(ptr)
    {}
};

class WeakNodeList
{
public:
    RefSpinLock m_lock;
    WeakNode* m_head;
    WeakNode* m_tail;
public:
    WeakNodeList()
    : m_head(NULL)
    , m_tail(NULL)
    {}
    ~WeakNodeList()
    {
        DeleteAllNodes_Locked();
    }
public:
    WeakNode* NewNode_NotLocked(RefObject* ptr);
    void DeleteNode_HalfLocked(WeakNode* node);
    void DeleteAllNodes_Locked();
    void SetWeakPtr(WeakNode* node,
                    WeakPtrBase* ptr);
};

class RefObject;

/// \brief WeakCounter
///
///
class WeakCounter : public MemObject
{
public:
    volatile RefObject* ref_object;
    volatile esint32 weak_count;
    RefSpinLock lock;
    ///while (!OSAtomicCompareAndSwap32((int32_t)0, (int32_t)1, (volatile int32_t*)lock))
    ///{}
    ///while (!OSAtomicCompareAndSwap32((int32_t)1, (int32_t)0, (volatile int32_t*)lock))
    ///{}
    ///esint32 lock;
    WeakCounter(RefObject* object)
    : ref_object(object)
    , weak_count(0)
    {}
    ~WeakCounter()
    {
    }
};

class RefObject
{
public:
    //////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize )
    {
        return NMalloc( nSize );
    }
	void operator delete( void *p)
	{
		Mfree(p);
	}
    //////////////////////////////////////////////////////////////////////////
    void* operator new( size_t nSize, const char* file,int line )
    {
        return _Malloc( nSize, file, line );
    }
    
	void operator delete( void *p, const char* file,int line )
    {
        Mfree(p);
    }
    //////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize, void* ptr )
	{
		EAssert(((ref_ptr)ptr % 16) == 0, "new object error");
		return ptr;
	}
    
	void operator delete( void *p, void* ptr )
	{
	}
    //////////////////////////////////////////////////////////////////////////
	void* operator new[]( size_t nSize )
	{
		return NMalloc( nSize );
	}
    
	void operator delete[]( void* ptr, size_t nSize )
	{
		Mfree(ptr);
	}
public:
	mutable volatile esint32 ref_count;
    mutable WeakCounter* weak_count;
	RefObject()
	{
		ref_count = 0;
        weak_count = VNEW WeakCounter(this);
        EAssert(weak_count, "weak count must be not null");
	}
    RefObject(const RefObject& obj)
    {
        ref_count = 0;
        weak_count = VNEW WeakCounter(this);
        EAssert(weak_count, "weak count must be not null");
    }
	virtual ~RefObject()
	{
        EAssert(weak_count, "weak count must be not null");
        volatile bool must_delete_count = false;
        {
            RefSpinLock::Instance inst = weak_count->lock.Lock();
            weak_count->ref_object = nullptr;
            if (!weak_count->weak_count) {
                weak_count->ref_object = nullptr;
                must_delete_count = true;
            }
        }
        if (must_delete_count) {
            delete weak_count;
            weak_count = nullptr;
        }
	}
    inline const RefObject& operator = (RefObject& obj) {
        EAssert(0, "RefObject can not perform assign operation");
        return *this;
    }
    inline euint32 GetRefCount() const {
        return ref_count;
    }
};
/**
namespace xhn {
    template< typename T,
    typename INC_CALLBACK,
    typename DEST_CALLBACK,
    typename GARBAGE_COLLECTOR >
    class SmartPtr;
}
**/

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
