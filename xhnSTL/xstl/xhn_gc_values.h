/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef __xhnSTL__xhn_gc_values__
#define __xhnSTL__xhn_gc_values__

#include "xhn_gc_container_base.h"

namespace xhn
{
    class GCInt : public GCObject
    {
    private:
        esint m_int;
    public:
        GCInt()
        : m_int(0)
        {}
        GCInt(esint i)
        : m_int(i)
        {}
        inline esint Get() const {
            return m_int;
        }
        inline void Set(esint i) {
            m_int = i;
        }
    };
    
    class GCUInt : public GCObject
    {
    private:
        euint m_uint;
    public:
        GCUInt()
        : m_uint(0)
        {}
        GCUInt(euint i)
        : m_uint(i)
        {}
        inline euint Get() const {
            return m_uint;
        }
        inline void Set(euint i) {
            m_uint = i;
        }
    };
    
    class GCFloat : public GCObject
    {
    private:
        float m_float;
    public:
        GCFloat()
        : m_float(0.0f)
        {}
        GCFloat(float f)
        : m_float(f)
        {}
        inline float Get() const {
            return m_float;
        }
        inline void Set(float f) {
            m_float = f;
        }
    };
    
    class GCDouble : public GCObject
    {
    private:
        double m_double;
    public:
        GCDouble()
        : m_double(0.0)
        {}
        GCDouble(double d)
        : m_double(d)
        {}
        inline double Get() const {
            return m_double;
        }
        inline void Set(double d) {
            m_double = d;
        }
    };
}

#endif /* defined(__xhnSTL__xhn_gc_values__) */

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

