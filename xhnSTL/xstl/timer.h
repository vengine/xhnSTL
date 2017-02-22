/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
inline double ms_to_s(double ms)
{
	return ms / 1000.0;
}
inline double s_to_ms(double s)
{
	return s * 1000.0;
}
inline double us_to_s(double us)
{
	return us / 1000000.0;
}
inline double s_to_us(double s)
{
	return s * 1000000.0;
}

class VTime
{
    friend class TimeCheckpoint;
private:
    double m_nanoTime;
public:
    VTime()
    : m_nanoTime(0.0)
    {}
    inline double GetNanosecond() const {
        return m_nanoTime;
    }
    inline double GetMicrosecond() const {
        return GetNanosecond() / 1000.0;
    }
    inline double GetMillisecond() const {
        return GetMicrosecond() / 1000.0;
    }
    inline double GetSecond() const {
        return GetMillisecond() / 1000.0;
    }
};
/**
#ifdef  __GNUC__
__inline__ uint64 perf_counter(void)
{
	euint32 lo, hi;
	// take time stamp counter, rdtscp does serialize by itself, and is much cheaper than using CPUID
	__asm__ __volatile__ (
		"rdtscp" : "=a"(lo), "=d"(hi)
		);
	return ((uint64)lo) | (((uint64)hi) << 32);
}
**/
#if defined(_MSC_VER) || defined(__MINGW32__)

inline euint64 perf_counter(void)
{
	euint64 ret;
	QueryPerformanceCounter((LARGE_INTEGER*)&ret);
	return ret;
}
inline double pref_frequency(void)
{
	LARGE_INTEGER temp;
	QueryPerformanceFrequency((LARGE_INTEGER*)&temp);
	return ((double)temp.QuadPart) / 1000000.0;
}
inline double calc_elapsed_time(euint64 start, euint64 end, double freq)
{
	return ((double)end - (double)start) / freq;
}
struct TimeCheckpoint
{
	euint64 timeStamp;
	double freq;
	TimeCheckpoint()
		: timeStamp(0)
		, freq(0.0)
	{}
	static inline TimeCheckpoint Tick() {
        TimeCheckpoint ret;
		ret.timeStamp = perf_counter();
		ret.freq = pref_frequency();
		return ret;
	}
	static inline double CalcElapsedTime(const TimeCheckpoint& prevCheckpoint, const TimeCheckpoint& curtCheckpoint) {
		double avFreq = (curtCheckpoint.freq + prevCheckpoint.freq) / 2.0;
        return calc_elapsed_time(prevCheckpoint.timeStamp, curtCheckpoint.timeStamp, avFreq);
	}
    static inline double CalcElapsedTimeInNano(const TimeCheckpoint& prevCheckpoint, const TimeCheckpoint& curtCheckpoint) {
        return CalcElapsedTime(prevCheckpoint, curtCheckpoint) * 1000.0;
    }
    static inline double Tock(const TimeCheckpoint& prevCheckpoint)
    {
        TimeCheckpoint curtCheckpoint = Tick();
        return CalcElapsedTime(prevCheckpoint, curtCheckpoint);
    }
    static inline void Tock(const TimeCheckpoint& prevCheckpoint, VTime& result)
    {
        TimeCheckpoint curtCheckpoint = Tick();
        result.m_nanoTime = CalcElapsedTimeInNano(prevCheckpoint, curtCheckpoint);
    }
};
#elif defined (LINUX)
#include<sys/time.h>
inline double calc_elapsed_time_in_us(struct timeval start, struct timeval end)
{
    return 1000000.0 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
}
struct TimeCheckpoint
{
    struct timeval timeStamp;
    TimeCheckpoint()
    {
        memset(&timeStamp, 0, sizeof(timeStamp));
    }
    static inline TimeCheckpoint Tick() {
        TimeCheckpoint ret;
		gettimeofday(&ret.timeStamp, nullptr);
		return ret;
	}
	static inline double CalcElapsedTime(const TimeCheckpoint& prevCheckpoint, const TimeCheckpoint& curtCheckpoint) {
        return calc_elapsed_time_in_us(prevCheckpoint.timeStamp, curtCheckpoint.timeStamp) / 1000000.0;
	}
	static inline double CalcElapsedTimeInNano(const TimeCheckpoint& prevCheckpoint, const TimeCheckpoint& curtCheckpoint) {
        return calc_elapsed_time_in_us(prevCheckpoint.timeStamp, curtCheckpoint.timeStamp) * 1000.0;
    }
    static inline double Tock(const TimeCheckpoint& prevCheckpoint)
    {
        TimeCheckpoint curtCheckpoint = Tick();
        return CalcElapsedTime(prevCheckpoint, curtCheckpoint);
    }
    static inline void Tock(const TimeCheckpoint& prevCheckpoint, VTime& result)
    {
        TimeCheckpoint curtCheckpoint = Tick();
        result.m_nanoTime = CalcElapsedTimeInNano(prevCheckpoint, curtCheckpoint);
    }
};
#elif defined (__APPLE__)
#include "TargetConditionals.h"
#    if TARGET_OS_IPHONE
#    include <MobileCoreServices/MobileCoreServices.h>
#    elif TARGET_OS_MAC
#    include <CoreServices/CoreServices.h>
#    endif
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>
///#include <kern/kern_types.h>
///#include <kern/clock.h>

#    if TARGET_OS_IPHONE
struct TimeCheckpoint
{
    uint64_t timeStamp;
    TimeCheckpoint(euint64 time)
    : timeStamp(time)
    {}
    TimeCheckpoint()
    : timeStamp(0)
    {}
    static inline TimeCheckpoint Tick() {
        TimeCheckpoint ret;
        ret.timeStamp = mach_absolute_time();
        return ret;
    }
    static inline double CalcElapsedTimeInNano(const TimeCheckpoint& prevCheckpoint, const TimeCheckpoint& curtCheckpoint) {
        uint64_t        elapsed;
        uint64_t        elapsedNano;
        static mach_timebase_info_data_t    sTimebaseInfo;
        elapsed = curtCheckpoint.timeStamp - prevCheckpoint.timeStamp;

        if ( sTimebaseInfo.denom == 0 ) {
            (void) mach_timebase_info(&sTimebaseInfo);
        }

        elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;

        uint64_t ret = elapsedNano;
        return (double)ret;
    }
    static inline double CalcElapsedTime(const TimeCheckpoint& prevCheckpoint, const TimeCheckpoint& curtCheckpoint) {
        return CalcElapsedTimeInNano(prevCheckpoint, curtCheckpoint) / 1000.0;
    }

    static inline double Tock(const TimeCheckpoint& prevCheckpoint)
    {
        TimeCheckpoint curtCheckpoint = Tick();
        return CalcElapsedTime(prevCheckpoint, curtCheckpoint);
    }
    static inline void Tock(const TimeCheckpoint& prevCheckpoint, VTime& result)
    {
        TimeCheckpoint curtCheckpoint = Tick();
        result.m_nanoTime = CalcElapsedTimeInNano(prevCheckpoint, curtCheckpoint);
    }
};
#    elif TARGET_OS_MAC
struct TimeCheckpoint
{
	uint64 timeStamp;
	TimeCheckpoint(euint64 time)
    : timeStamp(time)
    {}
	TimeCheckpoint()
    : timeStamp(0)
	{}
	static inline TimeCheckpoint Tick() {
        TimeCheckpoint ret;
		ret.timeStamp = mach_absolute_time();
		return ret;
	}
    static inline double CalcElapsedTimeInNano(const TimeCheckpoint& prevCheckpoint, const TimeCheckpoint& curtCheckpoint) {
        uint64        elapsed;
        uint64        elapsedNano;
        static mach_timebase_info_data_t    sTimebaseInfo;
        elapsed = curtCheckpoint.timeStamp - prevCheckpoint.timeStamp;

        if ( sTimebaseInfo.denom == 0 ) {
            (void) mach_timebase_info(&sTimebaseInfo);
        }

        elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;

        uint64 ret = elapsedNano;
        return (double)ret;
    }
	static inline double CalcElapsedTime(const TimeCheckpoint& prevCheckpoint, const TimeCheckpoint& curtCheckpoint) {
        return CalcElapsedTimeInNano(prevCheckpoint, curtCheckpoint) / 1000.0;
	}

    static inline double Tock(const TimeCheckpoint& prevCheckpoint)
    {
        TimeCheckpoint curtCheckpoint = Tick();
        return CalcElapsedTime(prevCheckpoint, curtCheckpoint);
    }
    static inline void Tock(const TimeCheckpoint& prevCheckpoint, VTime& result)
    {
        TimeCheckpoint curtCheckpoint = Tick();
        result.m_nanoTime = CalcElapsedTimeInNano(prevCheckpoint, curtCheckpoint);
    }
};
#    endif
#endif

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
