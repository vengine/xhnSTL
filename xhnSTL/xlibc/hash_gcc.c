/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "hash.h"
euint32 ELFHash( char   * str)
{
    euint32  hash  =   0 ;
    euint32  x     =   0 ;

    while  ( * str)
    {
        hash  =  (hash  <<   4 )  +  ( * str ++ );
        if  ((x  =  hash  &   0xF0000000L )  !=   0 )
        {
            hash  ^=  (x  >>   24 );
            hash  &=   ~ x;
        }
    }

    return  (hash  &   0x7FFFFFFF );
}

void init_hash_status(struct hash_calc_status* status)
{
    status->nr = 1;
    status->nr2 = 4;
}
void update_hash_status(struct hash_calc_status* status, const char* mem, euint length)
{
    while (length--)
    {
        status->nr^= (((status->nr & 63)+status->nr2)*((euint32) (euint8) *mem++))+ (status->nr << 8);
        status->nr2+=3;
    }
}
euint32 get_hash_value(struct hash_calc_status* status)
{
    return (status->nr & 0xfffffff0) | ((status->nr >> 16) & 0x0000000f);
}

euint32 calc_hashnr(const char *key, euint length)
{
	if (!length)
		return 0;
    register euint32 nr=1, nr2=4;
    while (length--)
    {
        nr^= (((nr & 63)+nr2)*((euint32) (euint8) *key++))+ (nr << 8);
        nr2+=3;
    }
    return (nr & 0xfffffff0) | ((nr >> 16) & 0x0000000f);
}

// Copyright (c) 2011 Google, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// CityHash, by Geoff Pike and Jyrki Alakuijala
//
// This file provides CityHash64() and related functions.
//
// It's probably possible to create even faster hash functions by
// writing a program that systematically explores some of the space of
// possible hash functions, by using SIMD instructions, or by
// compromising on hash quality.

static euint32 UNALIGNED_LOAD32(const char *p) {
    euint32 result;
    memcpy(&result, p, sizeof(result));
    return result;
}

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif

#else

#include <byteswap.h>

#endif

#ifdef WORDS_BIGENDIAN
#define uint32_in_expected_order(x) (bswap_32(x))
#define uint64_in_expected_order(x) (bswap_64(x))
#else
#define uint32_in_expected_order(x) (x)
#define uint64_in_expected_order(x) (x)
#endif

#if !defined(LIKELY)
#if HAVE_BUILTIN_EXPECT
#define LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define LIKELY(x) (x)
#endif
#endif

static euint32 Fetch32(const char *p) {
    return uint32_in_expected_order(UNALIGNED_LOAD32(p));
}

// Magic numbers for 32-bit hashing.  Copied from Murmur3.
static const euint32 c1 = 0xcc9e2d51;
static const euint32 c2 = 0x1b873593;

// A 32-bit to 32-bit integer hash copied from Murmur3.
static euint32 fmix(euint32 h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

static euint32 Rotate32(euint32 val, int shift) {
    // Avoid shifting by 32: doing so yields an undefined result.
    return shift == 0 ? val : ((val >> shift) | (val << (32 - shift)));
}

#undef PERMUTE3
#define PERMUTE3(a, b, c) do { std::swap(a, b); std::swap(a, c); } while (0)

static euint32 Mur(euint32 a, euint32 h) {
    // Helper from Murmur3 for combining two 32-bit values.
    a *= c1;
    a = Rotate32(a, 17);
    a *= c2;
    h ^= a;
    h = Rotate32(h, 19);
    return h * 5 + 0xe6546b64;
}

static euint32 Hash32Len13to24(const char *s, euint32 len) {
    euint32 a = Fetch32(s - 4 + (len >> 1));
    euint32 b = Fetch32(s + 4);
    euint32 c = Fetch32(s + len - 8);
    euint32 d = Fetch32(s + (len >> 1));
    euint32 e = Fetch32(s);
    euint32 f = Fetch32(s + len - 4);
    euint32 h = len;
    
    return fmix(Mur(f, Mur(e, Mur(d, Mur(c, Mur(b, Mur(a, h)))))));
}

static euint32 Hash32Len0to4(const char *s, euint32 len) {
    euint32 b = 0;
    euint32 c = 9;
    euint32 i = 0;
    for (; i < len; i++) {
        signed char v = s[i];
        b = b * c1 + v;
        c ^= b;
    }
    return fmix(Mur(b, Mur(len, c)));
}

static euint32 Hash32Len5to12(const char *s, euint32 len) {
    euint32 a = len, b = len * 5, c = 9, d = b;
    a += Fetch32(s);
    b += Fetch32(s + len - 4);
    c += Fetch32(s + ((len >> 1) & 4));
    return fmix(Mur(c, Mur(b, Mur(a, d))));
}

euint32 calc_cityhash32(const char *s, euint32 len) {
    if (len <= 24) {
        return len <= 12 ?
        (len <= 4 ? Hash32Len0to4(s, len) : Hash32Len5to12(s, len)) :
        Hash32Len13to24(s, len);
    }
    
    // len > 24
    euint32 h = len, g = c1 * len, f = g;
    euint32 a0 = Rotate32(Fetch32(s + len - 4) * c1, 17) * c2;
    euint32 a1 = Rotate32(Fetch32(s + len - 8) * c1, 17) * c2;
    euint32 a2 = Rotate32(Fetch32(s + len - 16) * c1, 17) * c2;
    euint32 a3 = Rotate32(Fetch32(s + len - 12) * c1, 17) * c2;
    euint32 a4 = Rotate32(Fetch32(s + len - 20) * c1, 17) * c2;
    h ^= a0;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    h ^= a2;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    g ^= a1;
    g = Rotate32(g, 19);
    g = g * 5 + 0xe6546b64;
    g ^= a3;
    g = Rotate32(g, 19);
    g = g * 5 + 0xe6546b64;
    f += a4;
    f = Rotate32(f, 19);
    f = f * 5 + 0xe6546b64;
    euint iters = (len - 1) / 20;
    do {
        euint32 a0 = Rotate32(Fetch32(s) * c1, 17) * c2;
        euint32 a1 = Fetch32(s + 4);
        euint32 a2 = Rotate32(Fetch32(s + 8) * c1, 17) * c2;
        euint32 a3 = Rotate32(Fetch32(s + 12) * c1, 17) * c2;
        euint32 a4 = Fetch32(s + 16);
        h ^= a0;
        h = Rotate32(h, 18);
        h = h * 5 + 0xe6546b64;
        f += a1;
        f = Rotate32(f, 19);
        f = f * c1;
        g += a2;
        g = Rotate32(g, 18);
        g = g * 5 + 0xe6546b64;
        h ^= a3 + a1;
        h = Rotate32(h, 19);
        h = h * 5 + 0xe6546b64;
        g ^= a4;
        g = bswap_32(g) * 5;
        h += a4 * 5;
        h = bswap_32(h);
        f += a0;
        ///PERMUTE3(f, h, g);
        {
            euint32
            t = f;
            f = h;
            h = t;
            ///
            t = f;
            f = g;
            g = t;
        }
        s += 20;
    } while (--iters != 0);
    g = Rotate32(g, 11) * c1;
    g = Rotate32(g, 17) * c1;
    f = Rotate32(f, 11) * c1;
    f = Rotate32(f, 17) * c1;
    h = Rotate32(h + g, 19);
    h = h * 5 + 0xe6546b64;
    h = Rotate32(h, 17) * c1;
    h = Rotate32(h + f, 19);
    h = h * 5 + 0xe6546b64;
    h = Rotate32(h, 17) * c1;
    return h;
}

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
