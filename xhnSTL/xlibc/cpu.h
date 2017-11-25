/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef CPU_H
#define CPU_H

#include "common.h"
#include "etypes.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
/// 这段代码来自 http://yyshen.github.io/2015/01/18/binding_threads_to_cores_osx.html
    
typedef struct cpu_set {
    uint32_t    count;
} cpu_set_t;
    
static inline void
CPU_ZERO(cpu_set_t *cs) { cs->count = 0; }
    
static inline void
CPU_SET(int num, cpu_set_t *cs) { cs->count |= (1 << num); }
    
static inline int
CPU_ISSET(int num, cpu_set_t *cs) { return (cs->count & (1 << num)); }
    
API_EXPORT int sched_getaffinity(pid_t pid, size_t cpu_size, cpu_set_t *cpu_set);
API_EXPORT int pthread_setaffinity_np(pthread_t thread, size_t cpu_size,
                                      cpu_set_t *cpu_set);
    
API_EXPORT euint32 number_of_cores(void);
API_EXPORT euint32 number_of_physicalcores(void);
    
API_EXPORT void nanopause(euint n);
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
