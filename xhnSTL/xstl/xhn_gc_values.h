//
//  xhn_gc_values.h
//  xhnSTL
//
//  Created by 徐海宁 on 15/1/5.
//  Copyright (c) 2015年 徐 海宁. All rights reserved.
//

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
