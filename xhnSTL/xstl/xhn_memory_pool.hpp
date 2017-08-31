/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_MEMORY_POOL_HPP
#define XHN_MEMORY_POOL_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "emem.h"

#include "xhn_memory.hpp"
#include "xhn_list.hpp"
#include "xhn_vector.hpp"

namespace xhn
{

template<typename T>
class memory_pool
{
private:
    MemPoolNode m_pool;
    euint m_count;
    euint m_capacity;
public:
    memory_pool()
    {
        m_pool = MemPoolNode_new(&g_DefaultMemoryAllcator, 16, sizeof(T));
        m_count = 0;
        m_capacity = 16;
    }
    ~memory_pool()
    {
        MemPoolNode_delete(m_pool);
    }
    T* alloc()
    {
        T* ret = nullptr;
        if (m_count < m_capacity) {
            ret = MemPoolNode_alloc(m_pool, false);
        }
        else {
            ret = NMalloc(sizeof(T));
        }
        m_count++;
        return ret;
    }
    void free(T* ptr)
    {
        if (m_count <= m_capacity || MemPoolNode_is_from(ptr)) {
            MemPoolNode_free(m_pool, ptr);
        }
        else {
            Mfree(ptr);
        }
    }
    void cleanup()
    {
        if (m_count > m_capacity) {
            MemPoolNode_delete(m_pool);
            m_pool = MemPoolNode_new(&g_DefaultMemoryAllcator, m_capacity * 2, sizeof(T));
            m_capacity *= 2;
        }
        m_count = 0;
    }
};
    
class fixed_memory_pool : public MemObject
{
private:
    MemPoolNode m_pool;
    euint m_count;
    euint m_capacity;
    euint m_chunk_size;
public:
    fixed_memory_pool(euint chunk_size)
    {
        m_pool = MemPoolNode_new(&g_DefaultMemoryAllcator, 16, chunk_size);
        m_count = 0;
        m_capacity = 16;
        m_chunk_size = chunk_size;
    }
    ~fixed_memory_pool()
    {
        MemPoolNode_delete(m_pool);
    }
    void* alloc()
    {
        T* ret = nullptr;
        if (m_count < m_capacity) {
            ret = MemPoolNode_alloc(m_pool, false);
        }
        else {
            ret = NMalloc(m_chunk_size);
        }
        m_count++;
        return ret;
    }
    void free(void* ptr)
    {
        if (m_count <= m_capacity || MemPoolNode_is_from(ptr)) {
            MemPoolNode_free(m_pool, ptr);
        }
        else {
            Mfree(ptr);
        }
    }
    void cleanup()
    {
        if (m_count > m_capacity) {
            MemPoolNode_delete(m_pool);
            m_pool = MemPoolNode_new(&g_DefaultMemoryAllcator, m_capacity * 2, m_chunk_size);
            m_capacity *= 2;
        }
        m_count = 0;
    }
    euint chunk_size() const {
        return m_chunk_size;
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
