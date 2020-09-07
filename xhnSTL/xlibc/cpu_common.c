/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "cpu.h"

#if defined(__APPLE__)

#include <sys/sysctl.h>
#include <mach/mach_types.h>
#include <mach/thread_act.h>

/// 这段代码来自 http://yyshen.github.io/2015/01/18/binding_threads_to_cores_osx.html

#define SYSCTL_CORE_COUNT   "machdep.cpu.core_count"

int sched_getaffinity(pid_t pid, size_t cpu_size, cpu_set_t *cpu_set)
{
    int32_t core_count = 0;
    size_t  len = sizeof(core_count);
    int ret = sysctlbyname(SYSCTL_CORE_COUNT, &core_count, &len, 0, 0);
    if (ret) {
        printf("error while get core count %d\n", ret);
        return -1;
    }
    cpu_set->count = 0;
    for (int i = 0; i < core_count; i++) {
        cpu_set->count |= (1 << i);
    }
    
    return 0;
}

int pthread_setaffinity_np(pthread_t thread, size_t cpu_size,
                           cpu_set_t *cpu_set)
{
    thread_port_t mach_thread;
    int core = 0;
    
    for (core = 0; core < 8 * cpu_size; core++) {
        if (CPU_ISSET(core, cpu_set)) break;
    }
    printf("binding to core %d\n", core);
    thread_affinity_policy_data_t policy = { core };
    mach_thread = pthread_mach_thread_np(thread);
    thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                      (thread_policy_t)&policy, 1);
    return 0;
}

euint32 number_of_cores()
{
    euint32 ncores;
    size_t len = sizeof(ncores);
    sysctlbyname("hw.ncpu", &ncores, &len, NULL, 0);
    
    return ncores;
}

euint32 number_of_physicalcores()
{
    euint32 ncores;
    size_t len = sizeof(ncores);
    sysctlbyname("hw.physicalcpu", &ncores, &len, NULL, 0);
    
    return ncores;
}
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <assert.h>
euint32 number_of_cores()
{
    unsigned logicalcpucount;

    SYSTEM_INFO systeminfo;
    GetSystemInfo( &systeminfo );

    logicalcpucount = systeminfo.dwNumberOfProcessors;
    return logicalcpucount;
}

// 这段代码来自于 https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
euint32 number_of_physicalcores()
{
	DWORD length = 0;
	const BOOL result_first = GetLogicalProcessorInformationEx(RelationProcessorCore, NULL, &length);
	assert(GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	unsigned char* buffer = (unsigned char*)malloc(length);
	const PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info =
	(const PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)(buffer);

	const BOOL result_second = GetLogicalProcessorInformationEx(RelationProcessorCore, info, &length);
	assert(result_second != FALSE);

	size_t nb_physical_cores = 0;
	size_t offset = 0;
	do {
		const PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX current_info =
		(const PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)(buffer + offset);
		offset += current_info->Size;
		++nb_physical_cores;
	} while (offset < length);

	free(buffer);

	return nb_physical_cores;
}
#elif defined(LINUX) && !defined(ANDROID)
#include <unistd.h>
euint32 number_of_cores()
{
    unsigned logicalcpucount;

    logicalcpucount = sysconf( _SC_NPROCESSORS_ONLN );
    return logicalcpucount;
}
euint32 number_of_physicalcores()
{
    euint32 registers[4];
    unsigned logicalcpucount;
    unsigned physicalcpucount;

    logicalcpucount = sysconf( _SC_NPROCESSORS_ONLN );
    __asm__ __volatile__ ("cpuid " :
                          "=a" (registers[0]),
                          "=b" (registers[1]),
                          "=c" (registers[2]),
                          "=d" (registers[3])
                          : "a" (1), "c" (0));
    unsigned CPUFeatureSet = registers[3];
    bool hyperthreading = CPUFeatureSet & (1 << 28);
    if (hyperthreading){
        physicalcpucount = logicalcpucount / 2;
    } else {
        physicalcpucount = logicalcpucount;
    }
    return physicalcpucount;
}
#else

euint32 number_of_cores()
{
    return 2;
}
euint32 number_of_physicalcores()
{
    return 2;
}

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
