/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef RWBUFFER_H
#define RWBUFFER_H
#include "common.h"
#include "etypes.h"
#include "emem.h"
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _rw_buffer* RWBuffer;

#define GetRealSize(type, size) size + ( sizeof(type) - (size % sizeof(type)) )

XHN_EXPORT RWBuffer RWBuffer_new(native_memory_allocator* allocator, euint buffer_size);
XHN_EXPORT void RWBuffer_delete(native_memory_allocator* allocator, RWBuffer _self);
XHN_EXPORT bool RWBuffer_Read(RWBuffer _self, euint* result, euint* read_size);
XHN_EXPORT bool RWBuffer_Write(RWBuffer _self, const euint* from, const euint write_size);
XHN_EXPORT bool RWBuffer_IsEmpty(RWBuffer _self);
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