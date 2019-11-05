/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef STACK_H
#define STACK_H
#include "common.h"
#include "etypes.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define STACK_INITIAL_LENGTH 4
struct _stack;
typedef struct _stack* Stack;
Stack Stack_new(etype _value_type);
Stack Stack_Init(Stack _stk, etype _value_type);
void Stack_Dest(Stack _stk);
void Stack_push(Stack _stk, var _data);
bool Stack_pop(Stack _stk, var* _to);
bool Stack_get_top(Stack _stk, var* _to);
void Stack_clear(Stack _stk);
void Stack_resize(Stack _stk, euint _size);
euint Stack_count(Stack _stk);
var* Stack_find(Stack _stk, euint _i);

struct _fixed_stack;
typedef struct _fixed_stack* FixedStack;
FixedStack FixedStack_new(euint _value_size);
void FixedStack_Dest(FixedStack _stk);
void FixedStack_push(FixedStack _stk, void* _data);
bool FixedStack_pop(FixedStack _stk, void* _to);
bool FixedStack_get_top(FixedStack _stk, void* _to);
void* FixedStack_get_top_ptr(FixedStack _stk);
bool FixedStack_walk_begin(FixedStack _stk, void* _to, euint* _top_ptr);
bool FixedStack_walk(FixedStack _stk, void* _to, euint* _top_ptr);
void FixedStack_clear(FixedStack _stk);
void FixedStack_resize(FixedStack _stk, euint _size);
euint FixedStack_count(FixedStack _stk);
void* FixedStack_find(FixedStack _stk, euint _i);

void FixedStack_test(void);
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
