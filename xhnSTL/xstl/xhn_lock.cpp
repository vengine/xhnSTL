/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "xhn_lock.hpp"
#include "xhn_atomic_operation.hpp"

///==========================================================================///
///  RecursiveSpinLock                                                       ///
///==========================================================================///

xhn::RecursiveSpinLock::Instance::~Instance()
{
    ELock_lock(&m_prototype->m_interlock);
    if(!AtomicDecrement(&m_prototype->m_lock)) {
        m_prototype->m_tid = 0;
    }
    ELock_unlock(&m_prototype->m_interlock);
}

xhn::RecursiveSpinLock::Instance xhn::RecursiveSpinLock::Lock()
{
    ELock_lock(&m_interlock);
    if (!m_tid) {
        m_tid = pthread_self();
    }
    else {
        if (m_tid != pthread_self()) {
            /// 这里先解锁了
            ELock_unlock(&m_interlock);
            ///
            while (1) {
                ELock_lock(&m_interlock);
                if (!m_tid) {
                    /// break以后实际上是锁住的
                    m_tid = pthread_self();
                    break;
                }
                ELock_unlock(&m_interlock);
            }
        }
    }
    AtomicIncrement(&m_lock);
    ELock_unlock(&m_interlock);
    return Instance(this);
}

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
