/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_UTILITY_HPP
#define XHN_UTILITY_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "hash.h"
#include "emem.h"
#include "xhn_atomic_operation.hpp"
inline euint _strlen(const char* s)
{
    euint count = 0;
    while (s[count])
        count++;
    return count;
}
#define to_struct_addr(t, member, addr) \
	(t*)( ((char*)addr) - (esint)(&((t*)0)->member) )
namespace xhn
{
class buffer
{
private:
    vptr m_buf;
public:
    inline buffer(euint size) {
        m_buf = NMalloc(size);
    }
    inline ~buffer() {
        Mfree(m_buf);
    }
    inline vptr get() {
        return m_buf;
    }
};
template<typename T>
euint _real_size()
{
    euint num_qwords = sizeof ( T ) / 16;
    euint mod = sizeof ( T ) % 16;

    if ( mod ) {
        num_qwords++;
    }

    return num_qwords * 16;
}

inline euint32 _hash ( char *&key )
{
    return calc_hashnr ( key, _strlen ( key ) );
}

inline euint32 _hash ( wchar_t *&key )
{
    int count = 0;
    while (key[count]) {
        count++;
    }
    return calc_hashnr ( (const char*)key, count * sizeof(wchar_t) );
}

template < typename InputIterator, typename OutputIterator, typename UnaryOperator >
OutputIterator transform ( InputIterator first1, InputIterator last1,
                           OutputIterator result, UnaryOperator op )
{
    while ( first1 != last1 ) {
        *result = op ( *first1 ); // or: *result=binary_op(*first1,*first2++);
        ++result;
        ++first1;
    }

    return result;
}

template < typename T >
struct FCtorProc {
    void operator () (T* ptr) {
        new ( ptr ) T ();
    }
    void operator () (T* dst, const T& src) {
        new ( dst ) T ( src );
    }
};

template < typename T >
struct FDestProc {
    void operator () (T* ptr) {
        ptr->~T();
    }
};

template <typename T>
struct FGetElementRealSizeProc {
    euint operator() () {
        euint num_qwords = sizeof ( T ) / 16;
        euint mod = sizeof ( T ) % 16;

        if ( mod ) {
            num_qwords++;
        }

        return num_qwords * 16;
    }
};

template <typename CHAR>
struct FGetCharRealSizeProc {
    euint operator() () {
        return sizeof(CHAR);
    }
};
    
template <typename T>
struct FGetElementRealSizeProcUnaligned {
    euint operator() () {
        return sizeof(T);
    }
};

inline char to_lower(char c)
{
    if (c >= 'A' && c <= 'Z') {
        char d = c - 'A';
        return 'a' + d;
    } else
        return c;
}

template<class T>
struct FLessThanProc
{
	bool operator()(const T& a, const T& b) const {
		return a < b;
	}
};
    
template<class T>
struct FGreaterThanProc
{
    bool operator()(const T& a, const T& b) const {
        return a > b;
    }
};

template <typename T>
class allocator
{
public:
    typedef euint size_type;
	typedef euint difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
    
	template<typename A>
	struct rebind
	{
		typedef allocator<A> other;
	};

	pointer address(reference v) const
	{	
		return &v;
	}

	const_pointer address(const_reference v) const
	{	
		return &v;
	}

	allocator()
	{
	}

	allocator(const allocator& rth) 
	{
	}

	template<class A>
	allocator(const allocator<A>&)
	{
	}

	template<class A>
	allocator<T>& operator=(const allocator<A>&)
	{
		return (*this);
	}

	void deallocate(pointer ptr, size_type)
	{	
		Mfree(ptr);
	}

	pointer allocate(size_type count)
	{
		return (pointer)NMalloc(count * sizeof(value_type));
	}

	pointer allocate(size_type count, const void*)
	{
        return (pointer)NMalloc(count * sizeof(value_type));
	}

	void construct(pointer ptr, const T& v)
	{	
		new ( ptr ) T ();
	}

	void construct(pointer ptr)
	{	
		new ( ptr ) T ();
	}

	void destroy(pointer ptr)
	{	
		ptr->~T();
	}

	size_type max_size() const {
		return static_cast<size_type>(-1) / sizeof(value_type);
	}
};
    
class AutolockGate
{
public:
    volatile esint32 m_passable;
public:
    AutolockGate()
    : m_passable(0)
    {}
    inline void open() {
        AtomicCompareExchange(0, 1, &m_passable);
    }
    inline bool pass() {
        return AtomicCompareExchange(1, 0, &m_passable);
    }
};
    
struct FWCharFormat
{
#if defined(_WIN32) || defined(_WIN64)
    wchar_t operator() (wchar_t wc) {
        if (wc == L'/')
            return L'\\';
        else
            return wc;
    }
#elif defined(__APPLE__)
    wchar_t operator() (wchar_t wc) {
        if (wc == L'\\')
            return L'/';
        else
            return wc;
    }
#else
#    error
#endif
};
    
struct FCharFormat
{
#if defined(_WIN32) || defined(_WIN64)
    char operator() (char c) {
        if (c == '/')
            return '\\';
        else
            return c;
    }
#elif defined(__APPLE__)
    char operator() (char c) {
        if (c == '\\')
            return '/';
        else
            return c;
    }
#else
#    error
#endif
};
    
inline void quick_sort(int* array, int begin, int end)
{
    int i = begin;
    int j = end;
    int x = array[begin];
    while (i < j)
    {
        while(i < j && array[j] >= x)
            j--;
        if(i < j) {
            array[i] = array[j];
            i++;
        }
        while(i < j && array[i] < x)
            i++;
        if(i < j) {
            array[j] = array[i];
            j--;
        }
    }
    array[i] = x;
    {
        if (i > begin)
            quick_sort(array, begin, i - 1);
        if (end > i)
            quick_sort(array, i + 1, end);
    }
}
template <typename RandomAccessIterator, typename Compare>
void sort_impl ( RandomAccessIterator first, RandomAccessIterator last, Compare& less )
{
    typename RandomAccessIterator::value_type x = *first;
    RandomAccessIterator i = first;
    RandomAccessIterator j = last;
    while (i < j)
    {
        bool cmp0 = less(x, *j);
        bool cmp1 = less(*j, x);
        while( i < j && (cmp0 || (!cmp0 && !cmp1)) ) {
            j--;
            cmp0 = less(x, *j);
            cmp1 = less(*j, x);
        }
        if(i < j) {
            *i = *j;
            i++;
        }
        while(i < j && less(*i, x))
            i++;
        if(i < j) {
            *j = *i;
            j--;
        }
    }
    *i = x;
    {
        if (i > first)
            sort_impl(first, i - 1, less);
        if (last > i)
            sort_impl(i + 1, last, less);
    }
}
template <typename RandomAccessIterator>
void sort ( RandomAccessIterator first, RandomAccessIterator last )
{
    if (first == last)
        return;
    last--;
    sort_impl(first, last, FLessThanProc<typename RandomAccessIterator::value_type>());
}
    
template <typename RandomAccessIterator, typename Compare>
void sort ( RandomAccessIterator first, RandomAccessIterator last, Compare comp )
{
    if (first == last)
        return;
    last--;
    sort_impl(first, last, comp);
}
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
