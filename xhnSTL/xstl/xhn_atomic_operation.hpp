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
inline esint32 AtomicIncrement(volatile esint32* i)
{
	return InterlockedIncrement((volatile LONG*)i);
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
	return InterlockedDecrement((volatile LONG*)i);
}
#elif defined (__APPLE__)
inline esint32 AtomicIncrement(volatile esint32* i)
{
	return OSAtomicIncrement32Barrier(i);
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
	return OSAtomicDecrement32Barrier(i);
}
inline esint64 AtomicIncrement64(volatile esint64* i)
{
	return OSAtomicIncrement64Barrier(i);
}
inline esint64 AtomicDecrement(volatile esint64* i)
{
	return OSAtomicDecrement64Barrier(i);
}
inline bool AtomicCompareExchange(esint32 oldValue, esint32 newValue, volatile esint32* theValue)
{
    return OSAtomicCompareAndSwap32Barrier(oldValue, newValue, theValue);
}
inline bool AtomicCompareExchangePtr(void* oldValue, void* newValue, void * volatile * theValue)
{
    return OSAtomicCompareAndSwapPtrBarrier(oldValue, newValue, theValue);
}
#define MemBarrier OSMemoryBarrier()
#elif defined(ANDROID) || defined(__ANDROID__)
///#include <sys/atomics.h>
inline esint32 AtomicIncrement(volatile esint32* i)
{
    return __sync_fetch_and_add(i, 1) + 1;
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
    return __sync_fetch_and_sub(i, 1) - 1;
}
inline bool AtomicCompareExchange(esint32 oldValue, esint32 newValue, volatile esint32* theValue)
{
    return __sync_val_compare_and_swap(theValue, oldValue, newValue);
}
#elif defined(LINUX) && defined(AO_ATOMIC_OPS_H)
inline esint32 AtomicIncrement(volatile esint32* i)
{
    return AO_fetch_and_add1((AO_t*)i);
}
inline esint32 AtomicDecrement(volatile esint32* i)
{
    return AO_fetch_and_sub1((AO_t*)i);
}
inline bool AtomicCompareExchange(esint32 oldValue, esint32 newValue, volatile esint32* theValue)
{
    return AO_compare_and_swap((AO_t*)theValue, oldValue, newValue);
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
