/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_ATOMIC_POINTER_HPP
#define XHN_ATOMIC_POINTER_HPP

#ifdef __cplusplus
#include "common.h"
#include "etypes.h"

#include "xhn_atomic_operation.hpp"

namespace xhn
{

template <typename T>
class AtomicPtr
{
public:
#if BIT_WIDTH == 32
    volatile esint32 m_ptr;
#else
    volatile esint64 m_ptr;
#endif
public:
    AtomicPtr()
    {
#if BIT_WIDTH == 32
        AtomicStore32(0, &m_ptr);
#else
        AtomicStore64(0, &m_ptr);
#endif
    }
    AtomicPtr(const AtomicPtr& ptr)
    {
#if BIT_WIDTH == 32
        AtomicStore32((esint32)ptr.get(), &m_ptr);
#else
        AtomicStore64((esint64)ptr.get(), &m_ptr);
#endif
    }
    AtomicPtr(T* ptr)
    {
#if BIT_WIDTH == 32
        AtomicStore32((esint32)ptr, &m_ptr);
#else
        AtomicStore64((esint64)ptr, &m_ptr);
#endif
    }
    ~AtomicPtr()
    {}
    inline bool operator < (const AtomicPtr& ptr) const
    {
        return get() < ptr.get();
    }
    inline bool operator > (const AtomicPtr& ptr) const
    {
        return get() > ptr.get();
    }
    inline bool operator == (const AtomicPtr& ptr) const
    {
        return get() == ptr.get();
    }
    inline bool operator != (const AtomicPtr& ptr) const
    {
        return get() != ptr.get();
    }
    inline T* operator = (T* ptr)
    {
#if BIT_WIDTH == 32
        AtomicStore32((esint32)ptr, &m_ptr);
#else
        AtomicStore64((esint64)ptr, &m_ptr);
#endif
        return ptr;
    }
    inline T* operator = (const AtomicPtr& ptr)
    {
#if BIT_WIDTH == 32
        AtomicStore32((esint32)ptr.get(), &m_ptr);
#else
        AtomicStore64((esint64)ptr.get(), &m_ptr);
#endif
        return get();
    }
    inline T* operator ->() {
#if BIT_WIDTH == 32
        return (T*)AtomicLoad32(&m_ptr);
#else
        return (T*)AtomicLoad64(&m_ptr);
#endif
    }
    inline const T* operator ->() const {
#if BIT_WIDTH == 32
        return (T*)AtomicLoad32(&m_ptr);
#else
        return (T*)AtomicLoad64(&m_ptr);
#endif
    }
    inline T& operator *() {
#if BIT_WIDTH == 32
        return *(T*)AtomicLoad32(&m_ptr);
#else
        return *(T*)AtomicLoad64(&m_ptr);
#endif
    }
    inline const T& operator *() const {
#if BIT_WIDTH == 32
        return *(const T*)AtomicLoad32(&m_ptr);
#else
        return *(const T*)AtomicLoad64(&m_ptr);
#endif
    }
    inline T* get()
    {
#if BIT_WIDTH == 32
        return (T*)AtomicLoad32(&m_ptr);
#else
        return (T*)AtomicLoad64(&m_ptr);
#endif
    }
    inline const T* get() const
    {
#if BIT_WIDTH == 32
        return (const T*)AtomicLoad32(&m_ptr);
#else
        return (const T*)AtomicLoad64(&m_ptr);
#endif
    }
    inline operator const T* () const {
#if BIT_WIDTH == 32
        return (T*)AtomicLoad32(&m_ptr);
#else
        return (T*)AtomicLoad64(&m_ptr);
#endif
    }
    inline operator T* () {
#if BIT_WIDTH == 32
        return (T*)AtomicLoad32(&m_ptr);
#else
        return (T*)AtomicLoad64(&m_ptr);
#endif
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
