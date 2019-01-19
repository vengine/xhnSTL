/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_MATH_HPP
#define XHN_MATH_HPP

#ifdef __cplusplus
#include <math.h>
#include "common.h"
#include "etypes.h"

namespace xhn {
    
#define XHN_PI 3.14159265358979323846264338327950288
#define XHN_E  2.718281828459045235360287471352662498
    
// (n + 1) * P(n + 1, x) = (2 * n + 1) * x * P(n, x) - n * P(n - 1, x)
//               (2 * n + 1) * x * P(n, x) - n * P(n - 1, x)
// P(n + 1, x) = -------------------------------------------
//                                (n + 1)
//
//                     (2 * (n - 1) + 1) * x * P((n - 1), x) - (n - 1) * P((n - 1) - 1, x)
// P((n - 1) + 1, x) = -------------------------------------------------------------------
//                                ((n - 1) + 1)
//
//                     (2 * n -       1) * x * P(n - 1  , x) - (n - 1) * P(n - 2      , x)
// P(n          , x) = -------------------------------------------------------------------
//                                (n)

template <typename T>
inline T P(unsigned int n, T x)
{
    if (0 == n) {
        return static_cast<T>(1.0) ;
    } else if (1 == n) {
        return x;
    }
    
    if (static_cast<T>(1.0) == x) {
        return static_cast<T>(1.0);
    }
    
    if (static_cast<T>(-1.0) == x) {
        return ((0 == n % 2) ? static_cast<T>(1.0) : static_cast<T>(-1.0)) ;
    }
    
    if ((x == static_cast<T>(0.0)) && (n % 2)) {
        return static_cast<T>(0.0);
    }
    
    T numerator =
    (static_cast<T>(2.0) * static_cast<T>(n) - static_cast<T>(1.0)) *
    x *
    P(n - 1, x) -
    static_cast<T>(n - 1) *
    P(n - 2, x);
    T denominator = static_cast<T>(n);
    return numerator / denominator;
}

inline double legendre( unsigned int n, double x )
{
    return P<double>(n, x);
}
inline double legendre( unsigned int n, float x )
{
    return static_cast<double>(P<float>(n, x));
}
inline double legendre( unsigned int n, long double x )
{
    return static_cast<double>(P<long double>(n, x));
}
inline float legendref( unsigned int n, float x )
{
    return P<float>(n, x);
}
long double legendrel( unsigned int n, long double x )
{
    return P<long double>(n, x);
}
    
inline double float_abs(double x)
{
    return fabs(x);
}
    
inline float float_abs(float x)
{
    return fabsf(x);
}
    
inline long double float_abs(long double x)
{
    return fabsl(x);
}
    
template <typename T>
bool float_equal(T a, T b, T tolerances)
{
    return float_abs(a - b) < tolerances;
}
    
inline double float_sqrt(double x)
{
    return sqrt(x);
}

inline float float_sqrt(float x)
{
    return sqrtf(x);
}

inline long double float_sqrt(long double x)
{
    return sqrtl(x);
}
    
// https://gist.github.com/jamesthompson/3815164
template <typename T>
inline T A(unsigned int l, unsigned int m, T x)
{
    T fact = static_cast<T>(1.0);
    T pll = static_cast<T>(0.0);
    T pmm = static_cast<T>(1.0);
    T pmmp1 = static_cast<T>(0.0);
    T somx2 = float_sqrt((static_cast<T>(1.0) - x)  * (static_cast<T>(1.0) + x));
    T calc = static_cast<T>(0.0);
    for (unsigned int i = 1; i <= m; i++) {
        pmm *= -fact * somx2;
        fact += 2.0;
    }
    if (l == m) {
        calc = pmm;
    } else {
        pmmp1 = x * static_cast<T>(2 * m + 1) * pmm;
        if ( (m + 1) == l ) {
            calc = pmmp1;
        } else {
            for (unsigned int ll = m + 2; ll <= l; ll++) {
                pll = (x * static_cast<T>(2 * ll - 1) * pmmp1 - static_cast<T>(ll + m - 1) * pmm) /
                       static_cast<T>(ll - m);
                pmm = pmmp1;
                pmmp1 = pll;
            }
            calc = pll;
        }
    }
    return calc;
}

inline double assoc_legendre( unsigned int n, unsigned int m, double x )
{
    return A<double>(n, m, x);
}
inline double assoc_legendre( unsigned int n, unsigned int m, float x )
{
    return static_cast<double>(A<float>(n, m, x));
}
inline double assoc_legendre( unsigned int n, unsigned int m, long double x )
{
    return static_cast<double>(A<long double>(n, m, x));
}
float assoc_legendref( unsigned int n, unsigned int m, float x )
{
    return A<float>(n, m, x);
}
long double assoc_legendrel( unsigned int n, unsigned int m, long double x )
{
    return A<long double>(n, m, x);
}
    
extern euint64 factorial_table[21];
inline euint64 factorial(unsigned int n)
{
    if (n <= 20) {
        return factorial_table[n];
    } else {
        return 0xffffffffffffffff;
    }
}
    
#if MAX_INT_SIZE > 64
    
inline void u128toa(euint128 v, char* s)
{
    char temp;
    int i = 0, j = 0;
    
    while(v > 0) {
        s[i++] = v % 10 + '0';
        v /= 10;
    }
    s[i] = '\0';

    i--;
    while(j < i) {
        temp = s[j];
        s[j] = s[i];
        s[i] = temp;
        j++;
        i--;
    }
}
extern euint128 factorial_table128[35];
inline euint128 factorial128(unsigned int n)
{
    if (n <= 34) {
        return factorial_table128[n];
    } else {
        return static_cast<euint128>(-1);
    }
}
    
inline float sigmoid(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

inline double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}
    
inline long double sigmoid(long double x)
{
    return static_cast<long double>(1.0) / (static_cast<long double>(1.0) + expl(-x));
}
    
inline float derivative_sigmoid(float x)
{
    return sigmoid(x) * (1.0f - sigmoid(x));
}

inline double derivative_sigmoid(double x)
{
    return sigmoid(x) * (1.0 - sigmoid(x));
}

inline long double derivative_sigmoid(long double x)
{
    return sigmoid(x) * (static_cast<long double>(1.0) - sigmoid(x));
}

#endif

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
