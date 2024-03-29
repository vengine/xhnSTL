/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_ATOMIC_OPERATION_HPP
#define XHN_ATOMIC_OPERATION_HPP
#ifdef __cplusplus
#include "common.h"
#include "etypes.h"
#if defined (_WIN32) || defined (_WIN64)
#include <windows.h>
//#include <xatomic.h>
#include <atomic>
//#include <xxatomic>

inline esint32 AtomicIncrement(volatile esint32* i)
{
	volatile std::atomic_int32_t*  ai = (volatile std::atomic_int32_t*)i;
	return ai->fetch_add(1) + 1;
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
	volatile std::atomic_int32_t* ai = (volatile std::atomic_int32_t*)i;
	return ai->fetch_sub(1) - 1;
}
inline esint64 AtomicIncrement64(volatile esint64* i)
{
	volatile std::atomic_int64_t* ai = (volatile std::atomic_int64_t*)i;
	return ai->fetch_add(1) + 1;
}
inline esint64 AtomicDecrement64(volatile esint64* i)
{
	volatile std::atomic_int64_t* ai = (volatile std::atomic_int64_t*)i;
	return ai->fetch_sub(1) - 1;
}
inline bool AtomicCompareExchange(esint32 oldValue, esint32 newValue, volatile esint32* theValue)
{
	//return InterlockedExchange((volatile LONG*)theValue, newValue) == newValue;
	volatile std::atomic_int32_t* ai = (volatile std::atomic_int32_t*)theValue;
	return ai->compare_exchange_strong(oldValue, newValue);
}
inline bool AtomicCompareExchange(esint64 oldValue, esint64 newValue, volatile esint64* theValue)
{
	//return InterlockedExchange64((volatile LONG64*)theValue, newValue) == newValue;
	volatile std::atomic_int64_t* ai = (volatile std::atomic_int64_t*)theValue;
	return ai->compare_exchange_strong(oldValue, newValue);
}

inline bool AtomicCompareExchangePtr(void* oldValue, void* newValue, void* volatile* theValue)
{
	volatile std::atomic<void*>* ai = (volatile std::atomic<void*>*)theValue;
	return ai->compare_exchange_strong(oldValue, newValue);
}
#define MemBarrier std::atomic_thread_fence(std::memory_order_seq_cst)

#elif defined (__APPLE__)
#define USING_BUILTIN_ATOMIC 1
#if USING_BUILTIN_ATOMIC
#include <atomic>
#endif
inline esint32 AtomicIncrement(volatile esint32* i)
{
#if !USING_BUILTIN_ATOMIC
	return OSAtomicIncrement32Barrier(i);
#else
    ///esint32 ret;
    ///__atomic_fetch_add(i, 1, __ATOMIC_ACQ_REL);
    ///__atomic_load(i, &ret, __ATOMIC_SEQ_CST);
    return __atomic_fetch_add(i, 1, __ATOMIC_SEQ_CST) + 1;
    ///__atomic_load(i, &ret, __ATOMIC_SEQ_CST);
    ///return ret;
#endif
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
#if !USING_BUILTIN_ATOMIC
	return OSAtomicDecrement32Barrier(i);
#else
    ///esint32 ret;
    ///__atomic_fetch_sub(i, 1, __ATOMIC_ACQ_REL);
    ///__atomic_load(i, &ret, __ATOMIC_SEQ_CST);
    return __atomic_fetch_sub(i, 1, __ATOMIC_SEQ_CST) - 1;
    ///__atomic_load(i, &ret, __ATOMIC_SEQ_CST);
    ///return ret;
#endif
}
inline esint64 AtomicIncrement64(volatile esint64* i)
{
#if !USING_BUILTIN_ATOMIC
	return OSAtomicIncrement64Barrier(i);
#else
    ///esint64 ret;
    ///__atomic_fetch_add(i, 1, __ATOMIC_ACQ_REL);
    ///__atomic_load(i, &ret, __ATOMIC_SEQ_CST);
    return __atomic_fetch_add(i, 1, __ATOMIC_SEQ_CST) + 1;
    ///__atomic_load(i, &ret, __ATOMIC_SEQ_CST);
    ///return ret;
#endif
}
inline esint64 AtomicDecrement64(volatile esint64* i)
{
#if !USING_BUILTIN_ATOMIC
	return OSAtomicDecrement64Barrier(i);
#else
    ///esint64 ret;
    ///__atomic_fetch_sub(i, 1, __ATOMIC_ACQ_REL);
    ///__atomic_load(i, &ret, __ATOMIC_SEQ_CST);
    return __atomic_fetch_sub(i, 1, __ATOMIC_SEQ_CST) - 1;
    ///__atomic_load(i, &ret, __ATOMIC_SEQ_CST);
    ///return ret;
#endif
}
inline bool AtomicCompareExchange(esint32 oldValue, esint32 newValue, volatile esint32* theValue)
{
#if !USING_BUILTIN_ATOMIC
    return OSAtomicCompareAndSwap32Barrier(oldValue, newValue, theValue);
#else
    int succ = __ATOMIC_ACQ_REL;
    int fail = __ATOMIC_RELAXED;
    ///int succ = __ATOMIC_SEQ_CST;
    ///int fail = __ATOMIC_SEQ_CST;
    bool ret = __atomic_compare_exchange(theValue, &oldValue, &newValue,
                                         false,
                                         succ,
                                         fail);
    return ret;
#endif
}
inline bool AtomicCompareExchange(esint64 oldValue, esint64 newValue, volatile esint64* theValue)
{
#if !USING_BUILTIN_ATOMIC
    return OSAtomicCompareAndSwap64Barrier(oldValue, newValue, theValue);
#else
    int succ = __ATOMIC_ACQ_REL;
    int fail = __ATOMIC_RELAXED;
    ///int succ = __ATOMIC_SEQ_CST;
    ///int fail = __ATOMIC_SEQ_CST;
    bool ret = __atomic_compare_exchange(theValue, &oldValue, &newValue,
                                         false,
                                         succ,
                                         fail);
    return ret;
#endif
}
inline bool AtomicCompareExchangePtr(void* oldValue, void* newValue, void * volatile * theValue)
{
#if !USING_BUILTIN_ATOMIC
    return OSAtomicCompareAndSwapPtrBarrier(oldValue, newValue, theValue);
#else
    int succ = __ATOMIC_ACQ_REL;
    int fail = __ATOMIC_RELAXED;
    ///int succ = __ATOMIC_SEQ_CST;
    ///int fail = __ATOMIC_SEQ_CST;
    bool ret = __atomic_compare_exchange(theValue, &oldValue, &newValue,
                                         false,
                                         succ,
                                         fail);
    return ret;
#endif
}
#if !USING_BUILTIN_ATOMIC
#define MemBarrier OSMemoryBarrier()
#else
#define MemBarrier __atomic_thread_fence(__ATOMIC_SEQ_CST)
#endif
#elif defined(ANDROID) || defined(__ANDROID__)
#define MemBarrier __atomic_thread_fence(__ATOMIC_SEQ_CST)
///#include <sys/atomics.h>
inline esint32 AtomicIncrement(volatile esint32* i)
{
    return __sync_fetch_and_add(i, 1) + 1;
}
inline esint64 AtomicIncrement64(volatile esint64* i)
{
    return __sync_fetch_and_add(i, 1) + 1;
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
    return __sync_fetch_and_sub(i, 1) - 1;
}
inline esint64 AtomicDecrement64(volatile esint64* i)
{
    return __sync_fetch_and_sub(i, 1) - 1;
}
inline bool AtomicCompareExchange(esint32 oldValue, esint32 newValue, volatile esint32* theValue)
{
    return __sync_bool_compare_and_swap(theValue, oldValue, newValue);
}
inline bool AtomicCompareExchange(esint64 oldValue, esint64 newValue, volatile esint64* theValue)
{
    return __sync_bool_compare_and_swap(theValue, oldValue, newValue);
}
#if BIT_WIDTH == 32
inline bool AtomicCompareExchangePtr(void* oldValue, void* newValue, void * volatile * theValue)
{
    return __sync_bool_compare_and_swap((volatile esint32*)theValue, (esint32)oldValue, (esint32)newValue);
}
#else
inline bool AtomicCompareExchangePtr(void* oldValue, void* newValue, void * volatile * theValue)
{
    return __sync_bool_compare_and_swap((volatile esint64*)theValue, (esint64)oldValue, (esint64)newValue);
}
#endif
#elif defined(LINUX) && defined(AO_ATOMIC_OPS_H)
inline esint32 AtomicIncrement(volatile esint32* i)
{
    AO_fetch_and_add1((AO_t*)i);
    return AO_load((AO_t*)i);
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
    AO_fetch_and_sub1((AO_t*)i);
    return AO_load((AO_t*)i);
}
inline bool AtomicCompareExchange(esint32 oldValue, esint32 newValue, volatile esint32* theValue)
{
    return AO_compare_and_swap((AO_t*)theValue, oldValue, newValue);
}
#elif defined(LINUX) && !defined(AO_ATOMIC_OPS_H)
#define MemBarrier __atomic_thread_fence(__ATOMIC_SEQ_CST)
inline esint32 AtomicIncrement(volatile esint32* i)
{
    return __sync_fetch_and_add(i, 1) + 1;
}
inline esint32 AtomicIncrement32(volatile esint32* i)
{
    return __sync_fetch_and_add(i, 1) + 1;
}
inline esint64 AtomicIncrement64(volatile esint64* i)
{
    return __sync_fetch_and_add(i, 1) + 1;
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
    return __sync_fetch_and_sub(i, 1) - 1;
}
inline esint32 AtomicDecrement32(volatile esint32* i)
{
    return __sync_fetch_and_sub(i, 1) - 1;
}
inline esint64 AtomicDecrement64(volatile esint64* i)
{
    return __sync_fetch_and_sub(i, 1) - 1;
}
inline bool AtomicCompareExchange(esint32 oldValue, esint32 newValue, volatile esint32* theValue)
{
    return __sync_bool_compare_and_swap(theValue, oldValue, newValue);
}
inline bool AtomicCompareExchange(esint64 oldValue, esint64 newValue, volatile esint64* theValue)
{
    return __sync_bool_compare_and_swap(theValue, oldValue, newValue);
}
#if BIT_WIDTH == 32
inline bool AtomicCompareExchangePtr(void* oldValue, void* newValue, void * volatile * theValue)
{
    return __sync_bool_compare_and_swap((volatile esint32*)theValue, (esint32)oldValue, (esint32)newValue);
}
#else
inline bool AtomicCompareExchangePtr(void* oldValue, void* newValue, void * volatile * theValue)
{
    return __sync_bool_compare_and_swap((volatile esint64*)theValue, (esint64)oldValue, (esint64)newValue);
}
#endif
#else
#error
#endif

#if defined (_WIN32) || defined (_WIN64)
inline esint32 AtomicLoad32(const volatile esint32* ptr)
{
    esint32 _Value;
    
#if defined(_M_ARM) || defined(_M_ARM64)
    _Value = __iso_volatile_load32(ptr);
    _Memory_barrier();
    
#else
    _Value = *ptr;
#if _MSVC_STL_VERSION < 143
    _Compiler_barrier();
#else
    _ReadWriteBarrier();
#endif
#endif
    
    return (_Value);
}
inline void AtomicStore32(esint32 value, volatile esint32 *result)
{
#if defined(_M_ARM) || defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store32(result, value);
    
#else
#if _MSVC_STL_VERSION < 143
    _Compiler_barrier();
#else
    _ReadWriteBarrier();
#endif
    *result = value;
#endif
}
inline esint64 AtomicLoad64(const volatile esint64* ptr)
{
    return ReadAcquire64(ptr);
}
inline void AtomicStore64(esint64 value, volatile esint64* result)
{
    WriteRelease64(result, value);
}
#else
inline esint32 AtomicLoad32(const volatile esint32* ptr)
{
    esint32 ret;
    __atomic_load(ptr, &ret, __ATOMIC_ACQUIRE);
    return ret;
}
inline void AtomicStore32(esint32 value, volatile esint32* result)
{
    __atomic_store(result, &value, __ATOMIC_RELEASE);
}
inline esint64 AtomicLoad64(const volatile esint64* ptr)
{
    esint64 ret;
    __atomic_load(ptr, &ret, __ATOMIC_ACQUIRE);
    return ret;
}
inline void AtomicStore64(esint64 value, volatile esint64* result)
{
    __atomic_store(result, &value, __ATOMIC_RELEASE);
}
#endif

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
