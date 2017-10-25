/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_TRIPLE_BUFFER_HPP
#define XHN_TRIPLE_BUFFER_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_lock.hpp"

namespace xhn
{

template <typename T>
class triple_buffer
{
public:
    class prepare_instance
    {
        friend class triple_buffer;
    private:
        triple_buffer* m_prototype;
        inline prepare_instance(triple_buffer* buffer)
        : m_prototype(buffer)
        {}
    public:
        inline prepare_instance(const prepare_instance& inst)
        : m_prototype(inst.m_prototype)
        {}
        inline ~prepare_instance()
        {
            ELock_unlock(&m_prototype->m_prepare_lock);
        }
        inline T* operator->() {
            return m_prototype->m_prepare_buffer;
        }
        inline const T* operator->() const {
            return m_prototype->m_prepare_buffer;
        }
        inline T& operator*() {
            return *m_prototype->m_prepare_buffer;
        }
        inline const T& operator*() const {
            return *m_prototype->m_prepare_buffer;
        }
    };
    class current_instance
    {
        friend class triple_buffer;
    private:
        triple_buffer* m_prototype;
        inline current_instance(triple_buffer* buffer)
        : m_prototype(buffer)
        {}
    public:
        inline current_instance(const current_instance& inst)
        : m_prototype(inst.m_prototype)
        {}
        inline ~current_instance()
        {
            ELock_unlock(&m_prototype->m_current_lock);
        }
        inline T* operator->() {
            return m_prototype->m_current_buffer;
        }
        inline const T* operator->() const {
            return m_prototype->m_current_buffer;
        }
        inline T& operator*() {
            return *m_prototype->m_current_buffer;
        }
        inline const T& operator*() const {
            return *m_prototype->m_current_buffer;
        }
    };
private:
    ELock m_prepare_lock;
    ELock m_current_lock;
    T* m_prepare_buffer;
    volatile T* m_next_buffer;
    T* m_current_buffer;
    esint32 m_has_next_buffer;
public:
    triple_buffer(T* buffer0,
                  T* buffer1,
                  T* buffer2)
    : m_prepare_buffer(buffer0)
    , m_next_buffer(buffer1)
    , m_current_buffer(buffer2)
    , m_has_next_buffer(0)
    {
        ELock_Init(&m_prepare_lock);
        ELock_Init(&m_current_lock);
    }
    inline prepare_instance prepare_lock() const
    {
        ELock_lock(&m_prepare_lock);
        T* old_next_buffer = nullptr;
#if BIT_WIDTH == 64
        old_next_buffer = reinterpret_cast<T*>(AtomicLoad64(reinterpret_cast<volatile esint64*>(&m_next_buffer)));
#else
        old_next_buffer = reinterpret_cast<T*>(AtomicLoad64(reinterpret_cast<volatile esint32*>(&m_next_buffer)));
#endif
        if (!AtomicCompareExchangePtr(old_next_buffer, m_prepare_buffer, m_next_buffer)) {
#if BIT_WIDTH == 64
            old_next_buffer = reinterpret_cast<T*>(AtomicLoad64(reinterpret_cast<volatile esint64*>(&m_next_buffer)));
#else
            old_next_buffer = reinterpret_cast<T*>(AtomicLoad64(reinterpret_cast<volatile esint32*>(&m_next_buffer)));
#endif
        }
        m_prepare_buffer = old_next_buffer;
        AtomicStore32(1, &m_has_next_buffer);
        return prepare_instance(this);
    }
    inline current_instance current_lock() const
    {
        ELock_lock(&m_current_lock);
        if (AtomicLoad32(&m_has_next_buffer)) {
            T* old_next_buffer = nullptr;
#if BIT_WIDTH == 64
            old_next_buffer = reinterpret_cast<T*>(AtomicLoad64(reinterpret_cast<volatile esint64*>(&m_next_buffer)));
#else
            old_next_buffer = reinterpret_cast<T*>(AtomicLoad64(reinterpret_cast<volatile esint32*>(&m_next_buffer)));
#endif
            if (!AtomicCompareExchangePtr(old_next_buffer, m_current_buffer, m_next_buffer)) {
#if BIT_WIDTH == 64
                old_next_buffer = reinterpret_cast<T*>(AtomicLoad64(reinterpret_cast<volatile esint64*>(&m_next_buffer)));
#else
                old_next_buffer = reinterpret_cast<T*>(AtomicLoad64(reinterpret_cast<volatile esint32*>(&m_next_buffer)));
#endif
            }
            m_current_buffer = old_next_buffer;
            AtomicStore32(0, &m_has_next_buffer);
        }
        return current_instance(this);
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
