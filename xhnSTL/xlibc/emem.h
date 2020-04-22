/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef EMEM_H
#define EMEM_H
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "etypes.h"
#ifdef __cplusplus
extern "C"
{
#endif
#if BIT_WIDTH == 32
#    define ALLOC_INFO_RESERVED 16
#else
#    define ALLOC_INFO_RESERVED 32
#endif
#define REFER_INFO_RESERVED 16

#define _MEM_BLOCK_HEAD_SIZE_ sizeof(euint) * 8
    
extern XHN_EXPORT native_memory_allocator g_DefaultMemoryAllcator;

/// 必须确保所有分配出来的内存都是经过初始化的
XHN_EXPORT void TestInit(void);
/// 必须在启动多线程系统前完成引导
XHN_EXPORT void MInit(void);

XHN_EXPORT vptr _Malloc(euint _size, const char* _file, euint32 _line);
XHN_EXPORT vptr _SMalloc(euint _size, const char* _file, euint32 _line);
XHN_EXPORT void _Mfree(vptr _ptr, const char* _file, euint32 _line);

XHN_EXPORT void* TestAlloc(euint _size);
XHN_EXPORT void TestFree(void* _ptr);

#define NMalloc(s) _Malloc(s, __FILE__, __LINE__)
#define SMalloc(s) _SMalloc(s, __FILE__, __LINE__)
#define Mfree(p) _Mfree(p, __FILE__, __LINE__)

XHN_EXPORT void Mlog(void);
XHN_EXPORT void Mprint(void);
XHN_EXPORT bool MCheck(void);
XHN_EXPORT void MCheck2(void* checker, FnCheckMemory fnCheckMem);

XHN_EXPORT euint32 ETestMark(void);
XHN_EXPORT bool ETest(vptr ptr);
XHN_EXPORT vptr Ealloc(euint _size);
XHN_EXPORT vptr SEalloc(euint _size);
XHN_EXPORT void Efree(vptr _ptr);
XHN_EXPORT const char* Minfo(vptr _ptr, euint32* _line);
XHN_EXPORT euint Msize(vptr _ptr);

typedef struct _mem_pool_node_
{
    struct _mem_pool_node* self;
} MemPoolNode;
XHN_EXPORT MemPoolNode MemPoolNode_new(native_memory_allocator* _alloc, euint _chk_size, euint _num_chks);
XHN_EXPORT void MemPoolNode_delete(MemPoolNode _self);
XHN_EXPORT void* MemPoolNode_alloc(MemPoolNode _self, volatile esint64* _mem_stamp, bool _is_safe_alloc);
XHN_EXPORT bool MemPoolNode_free(MemPoolNode _self, void* _ptr);
XHN_EXPORT bool MemPoolNode_is_hotspot(MemPoolNode _self, volatile esint64* _mem_stamp);
XHN_EXPORT bool MemPoolNode_is_coldspot(MemPoolNode _self, volatile esint64* _mem_stamp);
XHN_EXPORT bool MemPoolNode_empty(MemPoolNode _self);
XHN_EXPORT bool MemPoolNode_full(MemPoolNode _self);
XHN_EXPORT bool MemPoolNode_is_from(MemPoolNode _self, void* _ptr);
XHN_EXPORT euint MemPoolNode_number_of_chunks(MemPoolNode _self);

typedef struct mem_pool mem_pool;

XHN_EXPORT mem_pool* MemPool_new(native_memory_allocator* _alloc, euint _chk_size);
XHN_EXPORT void MemPool_delete(mem_pool* _self);
XHN_EXPORT void* MemPool_alloc(mem_pool* _self,
                               Iterator* _iter,
                               bool _is_safe_alloc);
XHN_EXPORT void* MemPool_salloc(mem_pool* _self, Iterator* _iter);
XHN_EXPORT void MemPool_free(mem_pool* _self,
                             Iterator _iter,
                             void* _ptr);
XHN_EXPORT euint MemPool_chunk_size(mem_pool* _self);

typedef struct mem_allocator mem_allocator;

XHN_EXPORT mem_allocator* MemAllocator_new(native_memory_allocator* _alloc);
XHN_EXPORT void MemAllocator_delete(mem_allocator* _self);
XHN_EXPORT void* MemAllocator_alloc(mem_allocator* _self, euint _size, bool _is_safe_alloc);
XHN_EXPORT void* MemAllocator_salloc(mem_allocator* _self, euint _size);
XHN_EXPORT void MemAllocator_free(mem_allocator* _self, void* _ptr);
XHN_EXPORT void MemAllocator_log(mem_allocator* _self);
XHN_EXPORT bool MemAllocator_check(mem_allocator* _self);
XHN_EXPORT euint MemAllocator_get_alloced_mem_size(mem_allocator* _self);

typedef struct Unlocked_mem_allocator Unlocked_mem_allocator;

XHN_EXPORT Unlocked_mem_allocator* UnlockedMemAllocator_new(native_memory_allocator* _alloc);
XHN_EXPORT void UnlockedMemAllocator_delete(Unlocked_mem_allocator* _self);
XHN_EXPORT void* UnlockedMemAllocator_alloc(Unlocked_mem_allocator* _self, euint _size, bool _is_safe_alloc);
XHN_EXPORT void* UnlockedMemAllocator_salloc(Unlocked_mem_allocator* _self, euint _size);
XHN_EXPORT void UnlockedMemAllocator_free(Unlocked_mem_allocator* _self, void* _ptr);
XHN_EXPORT void UnlockedMemAllocator_log(Unlocked_mem_allocator* _self);
XHN_EXPORT bool UnlockedMemAllocator_check(Unlocked_mem_allocator* _self);
XHN_EXPORT euint UnlockedMemAllocator_get_alloced_mem_size(Unlocked_mem_allocator* _self);
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

