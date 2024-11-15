/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_VECTOR_HPP
#define XHN_VECTOR_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "xhn_memory.hpp"
#include <new>
#include <initializer_list>

#include "xhn_iterator.hpp"
#include "xhn_utility.hpp"
#include "xhn_lambda.hpp"
namespace xhn
{
    template < typename T >
    struct FVectorNextProc {
        void operator() ( T *from, T *&to, euint ele_real_size ) {
            to = ( T * ) ( ( char * ) from + ele_real_size );
        }
    };
    template < typename T >
    struct FVectorPrevProc {
        void operator() ( T *from, T *&to, euint ele_real_size ) {
            to = ( T * ) ( ( char * ) from - ele_real_size );
        }
    };
    template < typename T >
    struct FVectorNextProcUnaligned {
        void operator() ( T *from, T *&to, euint ele_real_size ) {
            to = from + 1;
        }
    };
    template < typename T >
    struct FVectorPrevProcUnaligned {
        void operator() ( T *from, T *&to, euint ele_real_size ) {
            to = from - 1;
        }
    };
template < typename T,
           typename GET_ELEM_REAL_SIZE = FGetElementRealSizeProcUnaligned<T>,
           typename NEXT = FVectorNextProcUnaligned<T>,
           typename PREV = FVectorPrevProcUnaligned<T>,
           typename CTOR = FCtorProc<T>,
           typename DEST = FDestProc<T> >
class vector : public RefObjectBase, public MemObject
{
public:
    enum for_each_status
    {
        RemoveAndContinue,
        KeepAndContinue,
        Stop,
    };
public:
    struct FReadProc {
        void operator() ( T *from, T &to ) {
            to = *from;
        }
    };
    struct FWriteProc {
        void operator() ( T *to, T &from ) {
            *to = from;
        }
    };
    
    template <typename Owner>
    struct FRedirectProc {
        void operator() ( Owner *owner, T *from, T *&to, euint ele_real_size, esint offset ) {
            char *begin = owner->m_begin_addr;
            char *end = owner->m_end_addr;
            char *ptr = ( char * ) from;
            ptr += offset * ( esint ) ele_real_size;

            if ( ptr < begin ) {
                ptr = begin;
            }

            if ( ptr > end ) {
                ptr = end;
            }

            to = ( T * )ptr;
        }
    };

    class iterator : public random_readwrite_iterator<
    T,
    FReadProc,
    FWriteProc,
    NEXT,
    PREV,
    FRedirectProc<vector>,
    vector>
    {
    public:
        typedef random_readwrite_iterator<
        T,
        FReadProc,
        FWriteProc,
        NEXT,
        PREV,
        FRedirectProc<vector>,
        vector> base_type;
        typedef std::ptrdiff_t difference_type;
        typedef typename remove_cv<T>::type value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef std::random_access_iterator_tag iterator_category;
        iterator ( vector<T, GET_ELEM_REAL_SIZE, NEXT, PREV> *owner, char *a, euint ele_real_size )
            : base_type
            ( ( T * ) a, ele_real_size, FReadProc(), FWriteProc(), NEXT(), PREV(), FRedirectProc<vector>(), owner )
        {}
        inline iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline iterator operator++ ( int ) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline iterator operator-- ( int ) {
            iterator tmp = *this;
            --*this;
            return tmp;
        }
        inline iterator operator + ( esint offs ) {
            iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline iterator operator - ( esint offs ) {
            iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
        inline iterator operator + ( esint offs ) const {
            iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline iterator operator - ( esint offs ) const {
            iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
        inline const iterator& operator += ( esint offs ) {
            this->redirect( offs );
            return *this;
        }
        inline const iterator& operator -= ( esint offs ) {
            this->redirect( -offs );
            return *this;
        }
        inline euint operator - ( iterator iter ) const {
            ref_ptr offs = (ref_ptr)base_type::m_ptr - (ref_ptr)iter.base_type::m_ptr;
            return offs / base_type::m_ele_real_size;
        }
    };
    class const_iterator : public const_random_readwrite_iterator<
    T,
    FReadProc,
    FWriteProc,
    NEXT,
    PREV,
    FRedirectProc<vector>,
    vector>
    {
    public:
        typedef const_random_readwrite_iterator<
        T,
        FReadProc,
        FWriteProc,
        NEXT,
        PREV,
        FRedirectProc<vector>,
        vector> base_type;
        typedef std::ptrdiff_t difference_type;
        typedef typename remove_cv<T>::type value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef std::random_access_iterator_tag iterator_category;
        const_iterator ( vector<T, GET_ELEM_REAL_SIZE, NEXT, PREV> *owner, char *a, euint ele_real_size )
            : base_type
            ( ( T * ) a, ele_real_size, FReadProc(), FWriteProc(), NEXT(), PREV(), FRedirectProc<vector>(), owner )
        {}
        inline const_iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline const_iterator operator++ ( int ) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline const_iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline const_iterator operator-- ( int ) {
            const_iterator tmp = *this;
            --*this;
            return tmp;
        }
        inline const_iterator operator + ( esint offs ) {
            const_iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline const_iterator operator - ( esint offs ) {
            const_iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
        inline const_iterator operator + ( esint offs ) const {
            const_iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline const_iterator operator - ( esint offs ) const {
            const_iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
        inline euint operator - ( const_iterator iter ) const {
            ref_ptr offs = (ref_ptr)base_type::m_ptr - (ref_ptr)iter.base_type::m_ptr;
            return offs / base_type::m_ele_real_size;
        }
    };
    
    class reverse_iterator : public random_readwrite_iterator<
    T,
    FReadProc,
    FWriteProc,
    PREV,
    NEXT,
    FRedirectProc<vector>,
    vector>
    {
    public:
        typedef random_readwrite_iterator<
        T,
        FReadProc,
        FWriteProc,
        PREV,
        NEXT,
        FRedirectProc<vector>,
        vector> base_type;
        typedef std::ptrdiff_t difference_type;
        typedef typename remove_cv<T>::type value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef std::random_access_iterator_tag iterator_category;
        reverse_iterator ( vector<T, GET_ELEM_REAL_SIZE, NEXT, PREV> *owner, char *a, euint ele_real_size )
        : base_type
        ( ( T * ) a, ele_real_size, FReadProc(), FWriteProc(), PREV(), NEXT(), FRedirectProc<vector>(), owner )
        {}
        inline reverse_iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline reverse_iterator operator++ ( int ) {
            reverse_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline reverse_iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline reverse_iterator operator-- ( int ) {
            reverse_iterator tmp = *this;
            --*this;
            return tmp;
        }
        inline reverse_iterator operator + ( esint offs ) {
            reverse_iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline reverse_iterator operator - ( esint offs ) {
            reverse_iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
        inline reverse_iterator operator + ( esint offs ) const {
            reverse_iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline reverse_iterator operator - ( esint offs ) const {
            reverse_iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
    };
    class const_reverse_iterator : public const_random_readwrite_iterator<
    T,
    FReadProc,
    FWriteProc,
    PREV,
    NEXT,
    FRedirectProc<vector>,
    vector>
    {
    public:
        typedef const_random_readwrite_iterator<
        T,
        FReadProc,
        FWriteProc,
        PREV,
        NEXT,
        FRedirectProc<vector>,
        vector> base_type;
        typedef std::ptrdiff_t difference_type;
        typedef typename remove_cv<T>::type value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef std::random_access_iterator_tag iterator_category;
        const_reverse_iterator ( vector<T, GET_ELEM_REAL_SIZE, NEXT, PREV> *owner, char *a, euint ele_real_size )
        : base_type
        ( ( T * ) a, ele_real_size, FReadProc(), FWriteProc(), PREV(), NEXT(), FRedirectProc<vector>(), owner )
        {}
        inline const_reverse_iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline const_reverse_iterator operator++ ( int ) {
            const_reverse_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline const_reverse_iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline const_reverse_iterator operator-- ( int ) {
            const_reverse_iterator tmp = *this;
            --*this;
            return tmp;
        }
        inline const_reverse_iterator operator + ( esint offs ) {
            const_reverse_iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline const_reverse_iterator operator - ( esint offs ) {
            const_reverse_iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
        inline const_reverse_iterator operator + ( esint offs ) const {
            const_reverse_iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline const_reverse_iterator operator - ( esint offs ) const {
            const_reverse_iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
    };

    euint m_totel_ele_count;
    euint m_ele_real_size;
    euint m_curt_ele_count;
    char *m_begin_addr;
    char *m_end_addr;
    GET_ELEM_REAL_SIZE m_get_elem_real_size;
    CTOR m_ctor;
    DEST m_dest;
    iterator begin() {
        return iterator ( this, m_begin_addr, m_ele_real_size );
    }
    iterator end() {
        return iterator ( this, m_end_addr, m_ele_real_size );
    }
    const_iterator begin() const {
        return const_iterator ( (vector*)this, m_begin_addr, m_ele_real_size );
    }
    const_iterator end() const {
        return const_iterator ( (vector*)this, m_end_addr, m_ele_real_size );
    }
    reverse_iterator rbegin() {
        return reverse_iterator ( this, m_end_addr - m_ele_real_size, m_ele_real_size );
    }
    reverse_iterator rend() {
        return reverse_iterator ( this, m_begin_addr - m_ele_real_size, m_ele_real_size );
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator ( (vector*)this, m_end_addr - m_ele_real_size, m_ele_real_size );
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator ( (vector*)this, m_begin_addr - m_ele_real_size, m_ele_real_size );
    }
    inline void push_back ( const T& v ) {
        euint curt_count = _get_size();

        if ( curt_count + 1 > m_totel_ele_count ) {
            reserve((curt_count + 1) * 8);
        }

        m_ctor( (T*)m_end_addr, v );
        m_end_addr += m_ele_real_size;
        m_curt_ele_count++;
    }
    inline void push_back ( T&& v ) {
        euint curt_count = _get_size();
        
        if ( curt_count + 1 > m_totel_ele_count ) {
            reserve((curt_count + 1) * 8);
        }
        
        m_ctor( (T*)m_end_addr, v );
        m_end_addr += m_ele_real_size;
        m_curt_ele_count++;
    }
    inline void pop_back() {
        if ( m_end_addr != m_begin_addr ) {
            m_dest( ( T * ) ( m_end_addr - m_ele_real_size ) );
            m_end_addr -= m_ele_real_size;
            m_curt_ele_count--;
        }
    }
    inline T &back() {
        euint i = size() - 1;
        euint offs = m_ele_real_size * i;
        char *ptr = m_begin_addr + offs;
        return * ( ( T * ) ptr );
    }
    inline T &front() {
        return * ( ( T * ) m_begin_addr );
    }
    inline const T &back() const {
        euint i = size() - 1;
        euint offs = m_ele_real_size * i;
        char *ptr = m_begin_addr + offs;
        return * ( ( T * ) ptr );
    }
    inline const T &front() const {
        return * ( ( T * ) m_begin_addr );
    }
    inline T &operator [] ( euint i ) {
        EDebugAssert(i < size(), "subscript out of bounds");
        euint offs = m_ele_real_size * i;
        char *ptr = m_begin_addr + offs;
        return * ( ( T * ) ptr );
    }
    inline const T &operator [] ( euint i ) const {
        EDebugAssert(i < size(), "subscript out of bounds");
        euint offs = m_ele_real_size * i;
        char *ptr = m_begin_addr + offs;
        return * ( ( T * ) ptr );
    }
    inline bool empty() const {
        return _get_size() == 0;
    }
    inline euint size() const {
        return _get_size();
    }
    inline euint _get_size() const {
        return m_curt_ele_count;
    }
    inline void clear() {
        euint curt_count = _get_size();
		char *src_ptr = m_begin_addr;
		for ( euint i = 0; i < curt_count; i++ ) {
			m_dest((T*)src_ptr);
			src_ptr += m_ele_real_size;
		}
        m_end_addr = m_begin_addr;
        m_curt_ele_count = 0;
    }
    inline void reserve(euint n) {
        if ( n > m_totel_ele_count ) {
            euint curt_count = _get_size();
            char *tmp = ( char * ) NMalloc ( m_ele_real_size * n );
            char *dst_ptr = tmp;
            char *src_ptr = m_begin_addr;

            for ( euint i = 0; i < curt_count; i++ ) {
                m_ctor((T*)dst_ptr, * ( ( T * ) src_ptr ));
                dst_ptr += m_ele_real_size;
                src_ptr += m_ele_real_size;
            }

            src_ptr = m_begin_addr;

            for ( euint i = 0; i < curt_count; i++ ) {
                m_dest((T*)src_ptr);
                src_ptr += m_ele_real_size;
            }

            Mfree ( m_begin_addr );
            m_begin_addr = tmp;
            m_end_addr = m_begin_addr + ( m_ele_real_size * curt_count );
            m_totel_ele_count = n;
        }
    }
    inline void resize(euint n) {
        reserve(n);
        euint curt_count = _get_size();
		if (curt_count > n) {
			char* src_ptr = m_end_addr = m_begin_addr + m_ele_real_size * n;
			for (euint i = n; i < curt_count; i++) {
				m_dest((T*)src_ptr);
				src_ptr += m_ele_real_size;
			}
			m_curt_ele_count = n;
		} else if (curt_count < n) {
			euint d = n - curt_count;
			for (euint i = 0; i < d; i++) {
				m_ctor((T*)m_end_addr);
				m_end_addr += m_ele_real_size;
			}
			m_curt_ele_count = n;
		}
    }
	inline void erase(iterator i) {
		if ((ref_ptr)i.m_ptr >= (ref_ptr)m_begin_addr && (ref_ptr)i.m_ptr < (ref_ptr)m_end_addr) {
            m_dest(i.m_ptr);
			iterator prev_iter = i;
			i++;
            euint num = end() - i;
            memmove((void*)prev_iter.m_ptr, (void*)i.m_ptr, num * m_ele_real_size);
			m_end_addr -= m_ele_real_size;
            m_curt_ele_count--;
		}
	}
    inline void remove_first(const vector& v) {
        for (auto& e : v) {
            for (auto i = begin(); i != end(); i++) {
                if (e == *i) {
                    erase(i);
                    break;
                }
            }
        }
    }
    inline bool remove_first(const T& v) {
        for (auto i = begin(); i != end(); i++) {
            if (v == *i) {
                erase(i);
                return true;
            }
        }
        return false;
    }
    inline iterator remove(iterator i) {
        if ((ref_ptr)i.m_ptr >= (ref_ptr)m_begin_addr && (ref_ptr)i.m_ptr < (ref_ptr)m_end_addr) {
            iterator ret = i;
            m_dest(i.m_ptr);
			iterator prev_iter = i;
            i++;
            euint num = end() - i;
            memmove((void*)prev_iter.m_ptr, (void*)i.m_ptr, num * m_ele_real_size);
			m_end_addr -= m_ele_real_size;
            m_curt_ele_count--;
            return ret;
		}
        else {
            return end();
        }
	}
	inline iterator insert(iterator i, const T& v) {
		if ((ref_ptr)i.m_ptr >= (ref_ptr)m_begin_addr && (ref_ptr)i.m_ptr < (ref_ptr)m_end_addr) {
			ref_ptr offset = (ref_ptr)i.m_ptr - (ref_ptr)m_begin_addr;
			reserve(_get_size() + 1);
			i = iterator(this, m_begin_addr + offset, m_ele_real_size);
			m_end_addr += m_ele_real_size;
            m_curt_ele_count++;
            iterator n = i + 1;
            euint num = end() - i - 1;
            memmove(n.m_ptr, i.m_ptr, num * m_ele_real_size);
			iterator dst = i;
			dst++;
            memcpy(dst.m_ptr, i.m_ptr, m_ele_real_size);
            m_ctor((T*)i.m_ptr);
			*i = v;
			return i;
		}
        else if ((ref_ptr)i.m_ptr == (ref_ptr)m_end_addr) {
            push_back(v);
            return iterator(this, m_end_addr - m_ele_real_size, m_ele_real_size);
        }
		return iterator(this, NULL, 0);
	}
    template <class InputIterator>
    inline void insert (iterator i, InputIterator first, InputIterator last)
    {
        if ((ref_ptr)i.m_ptr >= (ref_ptr)m_begin_addr && (ref_ptr)i.m_ptr < (ref_ptr)m_end_addr) {
            for (; first != last; first++) {
                const T& v = *first;
                ref_ptr offset = (ref_ptr)i.m_ptr - (ref_ptr)m_begin_addr;
                reserve(_get_size() + 1);
                i = iterator(this, m_begin_addr + offset, m_ele_real_size);
                m_end_addr += m_ele_real_size;
                m_curt_ele_count++;
                iterator n = i + 1;
                euint num = end() - i - 1;
                memmove(n.m_ptr, i.m_ptr, num * m_ele_real_size);
                iterator dst = i;
                dst++;
                memcpy(dst.m_ptr, i.m_ptr, m_ele_real_size);
                m_ctor((T*)i.m_ptr);
                *i = v;
                i++;
            }
        }
        else if ((ref_ptr)i.m_ptr == (ref_ptr)m_end_addr) {
            for (; first != last; first++) {
                const T& v = *first;
                push_back(v);
            }
        }
    }
	inline T* get() const {
		return (T*)m_begin_addr;
	}
	inline void _init(const vector& v) {
		euint size = m_get_elem_real_size();
		m_ele_real_size = size;
		m_totel_ele_count = v.m_totel_ele_count;
        m_curt_ele_count = v.m_curt_ele_count;
		m_begin_addr = ( char * ) NMalloc ( size * m_totel_ele_count );
		m_end_addr = m_begin_addr + (v.m_end_addr - v.m_begin_addr);
		euint count = (m_end_addr - m_begin_addr) / size;
		for (euint i = 0; i < count; i++) {
			euint offs = size * i;
			char *dst = m_begin_addr + offs;
			char *src = v.m_begin_addr + offs;
			m_ctor((T*)dst, *((T*)src));
		}
	}
	inline void _dest() {
		euint curt_count = _get_size();
		char *src_ptr = m_begin_addr;
		for ( euint i = 0; i < curt_count; i++ ) {
			m_dest((T*)src_ptr);
			src_ptr += m_ele_real_size;
		}
		Mfree ( m_begin_addr );
	}
	vector& operator = (const vector& v) {
        _dest();
		_init(v);
		return *this;
	}
    bool operator < (const vector& v) const {
        const_iterator _viter = v.begin();
        const_iterator _vend = v.end();
        const_iterator _iter = begin();
        const_iterator _end = end();
        for (; _iter != _end && _viter != _vend; _iter++, _viter++) {
            if (*_iter < *_viter)
                return true;
            else if (*_iter > *_viter)
                return false;
        }
        if (_iter != _end && _viter == _vend)
            return true;
        else
            return false;
    }
    bool operator == (const vector& v) const {
        const_iterator _viter = v.begin();
        const_iterator _vend = v.end();
        const_iterator _iter = begin();
        const_iterator _end = end();
        for (; _iter != _end && _viter != _vend; _iter++, _viter++) {
            if (*_iter != *_viter)
                return false;
        }
        if (_iter == _end && _viter == _vend)
            return true;
        else
            return false;
    }
    vector& append (const vector& v) {
        const_iterator i = v.begin();
        const_iterator e = v.end();
        for (; i != e; i++) {
            push_back(*i);
        }
        return *this;
    }
    void throw_front(iterator i) {
        if ((ref_ptr)i.m_ptr > (ref_ptr)m_begin_addr && (ref_ptr)i.m_ptr < (ref_ptr)m_end_addr) {
            T tmp = *i;
            m_dest(i.m_ptr);
            iterator prev_iter = i - 1;
            for (; i != begin(); i--, prev_iter--)
            {
                memcpy((void*)i.m_ptr, (void*)prev_iter.m_ptr, m_ele_real_size);
            }
            m_ctor( (T*)m_begin_addr, tmp );
        }
    }
    void throw_back(iterator i) {
        T tmp = *i;
        erase(i);
        push_back(tmp);
    }
    euint32 hash_value() const {
        ::hash_calc_status status;
        init_hash_status(&status);
        const_iterator i = begin();
        const_iterator e = end();
        for (; i != e; i++) {
            ::update_hash_status(&status, (const char*)&*i, sizeof(*i));
        }
        return get_hash_value(&status);
    }
    void update_hash_status( ::hash_calc_status* status ) const {
        const_iterator i = begin();
        const_iterator e = end();
        for (; i != e; i++) {
            ::update_hash_status(status, (const char*)&*i, sizeof(*i));
        }
    }
    vector() {
        euint size = m_get_elem_real_size();
        m_begin_addr = ( char * ) NMalloc ( size * 32 );
        m_end_addr = m_begin_addr;
        m_ele_real_size = size;
        m_totel_ele_count = 32;
        m_curt_ele_count = 0;
    }
    vector(int count) {
        euint size = m_get_elem_real_size();
        m_begin_addr = ( char * ) NMalloc ( size * count );
        m_end_addr = m_begin_addr;
        m_ele_real_size = size;
        m_totel_ele_count = count;
        m_curt_ele_count = 0;
    }
	vector(const vector& v) {
		_init(v);
	}
    vector(vector&& v) {
        m_begin_addr = v.m_begin_addr;
        m_end_addr = v.m_end_addr;
        m_ele_real_size = v.m_ele_real_size;
        m_totel_ele_count = v.m_totel_ele_count;
        m_curt_ele_count = v.m_curt_ele_count;
        v.m_begin_addr = NULL;
    }
    vector(std::initializer_list<T> il) {
        euint cnt = il.size();
        euint size = m_get_elem_real_size();
        m_begin_addr = ( char * ) NMalloc ( size * cnt * 2 );
        m_end_addr = m_begin_addr;
        m_ele_real_size = size;
        m_totel_ele_count = cnt * 2;
        m_curt_ele_count = 0;
        auto iter = il.begin();
        auto end = il.end();
        for (; iter != end; iter++) {
            push_back(*iter);
        }
    }
    template <class InputIterator>
    vector (InputIterator first, InputIterator last) {
        euint size = m_get_elem_real_size();
        m_begin_addr = ( char * ) NMalloc ( size * 32 );
        m_end_addr = m_begin_addr;
        m_ele_real_size = size;
        m_totel_ele_count = 32;
        m_curt_ele_count = 0;
        InputIterator iter = first;
        for (; iter != last; iter++) {
            push_back(*iter);
        }
        push_back(*last);
    }
    ~vector() {
        if (m_begin_addr) {
            _dest();
        }
    }
    void for_each(Lambda<void(T&, bool*)>& proc) {
        bool stop = false;
        iterator i = begin();
        iterator e = end();
        for (; i != e; i++) {
            proc(*i, &stop);
            if (stop == true)
                break;
        }
    }
    
    /// has_removed        = hr
    /// prev_removed_index = pri
    /// element_count      = ec
    /// i                  = i
    /// 0 1 2 3 4 5 6 7 8 9
    /// E X E E X E X E E E
    ///
    /// E                   : hr = false, pri = 0, ec = 1, i = 0
    /// E X                 : hr = true,  pri = 1, ec = 1, i = 1
    /// E X E               : hr = true,  pri = 1, ec = 1, i = 2
    /// E X E E             : hr = true,  pri = 1, ec = 1, i = 3
    /// E X E E X           : hr = true,  pri = 4, ec = 3, i = 4 (REMOVE, [2 ~ 3] -> [1 ~ 2])
    /// E X E E X E         : hr = true,  pri = 4, ec = 3, i = 5
    /// E X E E X E X       : hr = true,  pri = 6, ec = 4, i = 6 (REMOVE, 5 -> 3)
    /// E X E E X E X E     : hr = true,  pri = 6, ec = 4, i = 7
    /// E X E E X E X E E   : hr = true,  pri = 6, ec = 4, i = 8
    /// E X E E X E X E E E : hr = true,  pri = 6, ec = 4, i = 9
    /// E X E E X E X E E E : hr = true,  pri = 6, ec = 7, i = 9 (REMOVE, [7 ~ 9] -> [4 ~ 6])
    
    template <typename PROC>
    void for_each(PROC& proc) {
        euint ele_size = m_get_elem_real_size();
        euint count = (m_end_addr - m_begin_addr) / ele_size;
        bool has_removed = false;
        euint prev_removed_index = 0;
        euint element_count = 0;
        euint i = 0;
        for (; i < count; i++) {
            for_each_status s = proc(*reinterpret_cast<T*>((m_begin_addr + ele_size * i)));
            if (KeepAndContinue == s) {
                if (!has_removed) {
                    element_count++;
                }
                else {
                    /// do nothing
                }
            }
            else if (RemoveAndContinue == s) {
                if (has_removed) {
                    m_dest(reinterpret_cast<T*>(m_begin_addr + ele_size * i));
                    
                    euint moved_count = i - prev_removed_index - 1;
                    if (moved_count) {
                        memmove(m_begin_addr + ele_size * element_count,
                                m_begin_addr + ele_size * (prev_removed_index + 1),
                                moved_count * ele_size);
                        element_count += moved_count;
                    }
                    prev_removed_index = i;
                }
                else {
                    m_dest(reinterpret_cast<T*>(m_begin_addr + ele_size * i));
                    
                    has_removed = true;
                    prev_removed_index = i;
                }
            }
            else if (Stop == s) {
                break;
            }
        }
        if (has_removed) {
            euint moved_count = count - prev_removed_index - 1;
            if (moved_count) {
                memmove(m_begin_addr + ele_size * element_count,
                        m_begin_addr + ele_size * (prev_removed_index + 1),
                        moved_count * ele_size);
            }
            m_end_addr = m_begin_addr + ele_size * (element_count + moved_count);
            m_curt_ele_count = element_count + moved_count;
        }
    }
    void reset() {
        _dest();
        euint size = m_get_elem_real_size();
        m_begin_addr = ( char * ) NMalloc ( size * 32 );
        m_end_addr = m_begin_addr;
        m_ele_real_size = size;
        m_totel_ele_count = 32;
        m_curt_ele_count = 0;
    }
};
};

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
