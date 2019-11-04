/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef LIST_H
#define LIST_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "common.h"
#include "etypes.h"
struct _list;
typedef struct _list* List;
List List_new(etype _value_type, native_memory_allocator* _alloc);
List List_Init(List _lst, etype _value_type, native_memory_allocator* _alloc);
void List_Dest(List _lst);
Iterator List_push_back(List _lst, var _data);
Iterator List_push_front(List _lst, var _data);
Iterator List_insert_after(List _lst, Iterator _i, var _data);
Iterator List_insert_before(List _lst, Iterator _i, var _data);
Iterator List_remove(List _lst, Iterator _i);
Iterator List_next(Iterator _i);
Iterator List_prev(Iterator _i);
Iterator List_begin(List _lst);
Iterator List_end(List _lst);
List List_append(List _head, List _tail);
List List_break(List _lst, Iterator _i);
euint List_count(List _lst);
var List_get_value(Iterator _i);
void List_throw_front(List _lst, Iterator _i);
void List_throw_back(List _lst, Iterator _i);
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