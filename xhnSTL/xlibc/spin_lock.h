/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H
#include "common.h"
#include "etypes.h"
#ifdef __cplusplus
extern "C" {
#endif


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
typedef volatile esint32 ELock;
    #define ELock_Init(lock) *lock = 0
    /**
_INLINE_ void ELock_Init(ELock* lock) {
    *lock = 0;
}
     **/
_INLINE_ void ELock_lock(ELock* lock) {
	while (InterlockedCompareExchange((LONG volatile *)lock, (LONG)1, (LONG)0) == (LONG)1)
	{}
}
_INLINE_ void ELock_unlock(ELock* lock) {
	while (InterlockedCompareExchange((LONG volatile *)lock, (LONG)0, (LONG)1) == (LONG)0)
	{}
}
#elif defined(__APPLE__)
#ifndef OSSPINLOCK_DEPRECATED
typedef volatile OSSpinLock ELock;
#define ELock_Init(lock) *lock = OS_SPINLOCK_INIT
inline void ELock_lock(ELock* lock) {
    OSSpinLockLock(lock);
}
inline void ELock_unlock(ELock* lock) {
    OSSpinLockUnlock(lock);
}
inline bool ELock_try(ELock* lock) {
    return OSSpinLockTry(lock);
}
#else
#include <os/lock.h>
typedef os_unfair_lock ELock;
#define ELock_Init(lock) *lock = OS_UNFAIR_LOCK_INIT
inline void ELock_lock(ELock* lock) {
    os_unfair_lock_lock(lock);
}
inline void ELock_unlock(ELock* lock) {
    os_unfair_lock_unlock(lock);
}
inline bool ELock_try(ELock* lock) {
    return os_unfair_lock_trylock(lock);
}
#endif
#elif defined(LINUX) && defined(AO_ATOMIC_OPS_H)
    ///AO_compare_and_swap  takes an address, an old value and a new value and
    ///returns an int.  non-zero indicates the compare and swap succeeded.
    ///AO_INLINE int
    ///AO_compare_and_swap(volatile AO_t *addr, AO_t old, AO_t new_val)
    ///void AO_load(AO_t *addr)
    ///void AO_store(AO_t *addr, AO_t val)

    ///int AO_test_and_set (AO_t *addr)

    ///AO_t AO_fetch_and_add(AO_t *addr, AO_t incr)
    ///AO_t AO_fetch_and_add1(AO_t *addr)
    ///AO_t AO_fetch_and_sub1(AO_t *addr)

    ///void AO_or(AO_t *p, AO_t incr)
    ///int AO_compare_and_swap(AO_t *addr, AO_t old, AO_t new_val)
typedef volatile esint32 ELock;
#define ELock_Init(lock) *lock = 0
#define ELock_lock(lock) \
    while (!AO_compare_and_swap((AO_t*)lock, 0, 1)) \
    {}

#define ELock_unlock(lock) \
    while (!AO_compare_and_swap((AO_t*)lock, 1, 0)) \
    {}

#define ELock_try(lock) \
    AO_load((AO_t*)lock)

#endif

#ifdef __cplusplus
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
