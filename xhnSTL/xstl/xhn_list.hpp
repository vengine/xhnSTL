/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_LIST_H
#define XHN_LIST_H

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_iterator.hpp"
#include "xhn_memory.hpp"
#ifndef __APPLE__
#include <pthread.h>
#else
#define _SCHED_H
///#include </usr/include/sched.h>
///#include </usr/include/pthread.h>
#endif
namespace xhn
{
    template <typename T>
    class list_node : public MemObject
    {
    public:
        list_node *m_iter_prev;
        list_node *m_iter_next;
        ALIGN_FLAG ( 16 ) T m_value;
        list_node ( const T &data )
        : m_iter_prev ( NULL )
        , m_iter_next ( NULL )
        , m_value ( data )
        {}
    };
    template <typename T>
    class FListNodeAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef list_node<T>* pointer;
        typedef const list_node<T>* const_pointer;
        typedef list_node<T> value_type;
        typedef list_node<T>& reference;
        typedef const list_node<T>& const_reference;
        
        pointer address(reference v) const
        {
            return &v;
        }
        
        const_pointer address(const_reference v) const
        {
            return &v;
        }
        
        FListNodeAllocator()
        {
        }
        
        FListNodeAllocator& operator=(const FListNodeAllocator&)
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
        
        void construct(pointer ptr, const list_node<T>& v)
        {
            new (ptr) list_node<T>( v );
        }
        
        void construct(const pointer ptr)
        {
            new ( ptr ) list_node<T> ();
        }
        
        void construct(pointer ptr, const T& v)
        {
            new ( ptr ) list_node<T> (v);
        }
        
        void destroy(pointer ptr)
        {
            ptr->~list_node<T>();
        }
        
        size_type max_size() const {
            return static_cast<size_type>(-1) / sizeof(value_type);
        }
    };
template< typename T, typename ALLOCATOR = FListNodeAllocator<T> >
class list : public RefObject
{
public:
    typedef list_node<T> node_type;
    node_type *m_head;
    node_type *m_tail;
    euint m_count;
    ALLOCATOR m_allocator;
    list() {
        m_head = NULL;
        m_tail = NULL;
        m_count = 0;
    }
	list (const list& lt)
	{
		m_head = NULL;
		m_tail = NULL;
		m_count = 0;
		const_iterator iter = lt.begin();
		const_iterator end = lt.end();
		for (; iter != end; iter++)
		{
            push_back(*iter);
		}
	}
    ~list() {
        clear();
    }
    void clear() {
		node_type *node = m_head;

		while ( node ) {
			node_type *tmp = node;
			node = node->m_iter_next;
            m_allocator.destroy(tmp);
            m_allocator.deallocate(tmp, 1);
		}

		m_head = NULL;
		m_tail = NULL;
		m_count = 0;
    }

	list* clone() {
		list* ret = VNEW list(*this);
		return ret;
	}

    bool empty() const {
        return m_count == 0;
    }

    template<typename N>
    struct FReadProc {
        void operator() ( N *from, N &to ) {
            to = *from;
        }
    };
    template<typename N>
    struct FWriteProc {
        void operator() ( N *to, N &from ) {
            *to = from;
        }
    };
    template<typename N>
    struct FNextProc {
        void operator() ( N *from, N *&to, euint ele_real_size ) {
            to = from->m_iter_next;
        }
    };
    template<typename N>
    struct FPrevProc {
        void operator() ( N *from, N *&to, euint ele_real_size ) {
            to = from->m_iter_prev;
        }
    };

    class iterator : public bidirectional_readwrite_iterator<
    node_type,
    FReadProc<node_type>,
    FWriteProc<node_type>,
    FNextProc<node_type>,
    FPrevProc<node_type> >
    {
    public:
        typedef bidirectional_readwrite_iterator<
        node_type,
        FReadProc<node_type>,
        FWriteProc<node_type>,
        FNextProc<node_type>,
        FPrevProc<node_type> > base_type;
        iterator ( node_type *a, euint ele_real_size )
            : base_type (
                         a,
                         ele_real_size,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FNextProc<node_type>(),
                         FPrevProc<node_type>() )
        {}
		iterator (  )
			: base_type (
                         NULL,
                         0,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FNextProc<node_type>(),
                         FPrevProc<node_type>() )
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
        inline T &operator*() {
            return base_type::m_ptr->m_value;
        }
        inline T *operator->() {
            return &base_type::m_ptr->m_value;
        }
    };
    class const_iterator : public const_bidirectional_readwrite_iterator<
    node_type,
    FReadProc<node_type>,
    FWriteProc<node_type>,
    FNextProc<node_type>,
    FPrevProc<node_type> >
    {
    public:
        typedef const_bidirectional_readwrite_iterator<
        node_type,
        FReadProc<node_type>,
        FWriteProc<node_type>,
        FNextProc<node_type>,
        FPrevProc<node_type> > base_type;
        const_iterator ( node_type *a, euint ele_real_size )
            : base_type (
                         a,
                         ele_real_size,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FNextProc<node_type>(),
                         FPrevProc<node_type>() )
        {}
		const_iterator (  )
			: base_type (
                         NULL,
                         0,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FNextProc<node_type>(),
                         FPrevProc<node_type>() )
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
        inline const T &operator*() {
            return base_type::m_ptr->m_value;
        }
        inline const T *operator->() {
            return &base_type::m_ptr->m_value;
        }
    };
	class reverse_iterator : public bidirectional_readwrite_iterator<
    node_type,
    FReadProc<node_type>,
    FWriteProc<node_type>,
    FPrevProc<node_type>,
    FNextProc<node_type> >
	{
	public:
		typedef bidirectional_readwrite_iterator<
        node_type,
        FReadProc<node_type>,
        FWriteProc<node_type>,
        FPrevProc<node_type>,
        FNextProc<node_type> > base_type;
		reverse_iterator ( node_type *a, euint ele_real_size )
			: base_type (
                         a,
                         ele_real_size,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FPrevProc<node_type>(),
                         FNextProc<node_type>() )
		{}
		reverse_iterator (  )
			: base_type (
                         NULL, 0,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FPrevProc<node_type>(),
                         FNextProc<node_type>() )
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
		inline T &operator*() {
			return base_type::m_ptr->m_value;
		}
		inline T *operator->() {
			return &base_type::m_ptr->m_value;
		}
	};
	class const_reverse_iterator : public const_bidirectional_readwrite_iterator<
    node_type,
    FReadProc<node_type>,
    FWriteProc<node_type>,
    FPrevProc<node_type>,
    FNextProc<node_type> >
	{
	public:
		typedef const_bidirectional_readwrite_iterator<
        node_type,
        FReadProc<node_type>,
        FWriteProc<node_type>,
        FPrevProc<node_type>,
        FNextProc<node_type> > base_type;
		const_reverse_iterator ( node_type *a, euint ele_real_size )
			: base_type (
                         a,
                         ele_real_size,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FPrevProc<node_type>(),
                         FNextProc<node_type>() )
		{}
		const_reverse_iterator (  )
			: base_type (
                         NULL,
                         0,
                         FReadProc<node_type>(),
                         FWriteProc<node_type>(),
                         FPrevProc<node_type>(),
                         FNextProc<node_type>() )
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
		inline const T &operator*() {
			return base_type::m_ptr->m_value;
		}
		inline const T *operator->() {
			return &base_type::m_ptr->m_value;
		}
	};
public:
    iterator push_back ( const T &data ) {
        node_type *node = m_allocator.allocate(1);
        m_allocator.construct( node, data );
       
        if ( m_tail ) {
            m_tail->m_iter_next = node;
            node->m_iter_prev = m_tail;
            node->m_iter_next = NULL;
            m_tail = node;
        } else {
            m_head = m_tail = node;
            m_tail->m_iter_prev = NULL;
            m_tail->m_iter_next = NULL;
        }

        m_count++;
        iterator ret ( m_tail, 0 );
        return ret;
    }

    iterator push_front ( const T &data ) {
        node_type *node = m_allocator.allocate(1);
        m_allocator.construct( node, data );

        if ( m_head ) {
            m_head->m_iter_prev = node;
            node->m_iter_next = m_head;
            node->m_iter_prev = NULL;
            m_head = node;
        } else {
            m_tail = m_head = node;
            m_head->m_iter_prev = NULL;
            m_head->m_iter_next = NULL;
        }

        m_count++;
        iterator ret ( m_head, 0 );
        return ret;
    }
    iterator insert_after ( iterator i, const T &data ) {
        node_type *node = i.m_ptr;
        node_type *n = m_allocator.allocate(1);
        m_allocator.construct( n, data );

        if ( node->m_iter_next ) {
            n->m_iter_next = node->m_iter_next;
            node->m_iter_next->m_iter_prev = n;
            n->m_iter_prev = node;
            node->m_iter_next = n;
            m_count++;
            return iterator ( n, 0 );
        } else {
            node->m_iter_next = n;
            m_tail = node->m_iter_next;
            m_tail->m_iter_prev = node;
            m_tail->m_iter_next = NULL;
            m_count++;
            iterator ret ( m_tail, 0 );
            return ret;
        }
    }

    iterator insert_before ( iterator i, const T &data ) {
        node_type *node = i.m_ptr;
        node_type *n = m_allocator.allocate(1);
        m_allocator.construct( n, data );

        if ( node->m_iter_prev ) {
            n->m_iter_prev = node->m_iter_prev;
            node->m_iter_prev->m_iter_next = n;
            n->m_iter_next = node;
            node->m_iter_prev = n;
            m_count++;
            return iterator ( n, 0 );
        } else {
            node->m_iter_prev = n;
            m_head = node->m_iter_prev;
            m_head->m_iter_prev = NULL;
            m_head->m_iter_next = node;
            m_count++;
            iterator ret ( m_head, 0 );
            return ret;
        }
    }

    iterator remove ( iterator i ) {
        if ( !m_count ) {
            return iterator ( NULL, 0 );
        } else {
            node_type *node = i.m_ptr;
            m_count--;

            if ( node != m_head && node != m_tail ) {
                node_type *ret = node->m_iter_next;
                node->m_iter_prev->m_iter_next = node->m_iter_next;
                node->m_iter_next->m_iter_prev = node->m_iter_prev;
                m_allocator.destroy(node);
                m_allocator.deallocate(node, 1);
                return iterator ( ret, NULL );
            } else if ( node != m_head && node == m_tail ) {
                node->m_iter_prev->m_iter_next = NULL;
                m_tail = node->m_iter_prev;
                m_allocator.destroy(node);
                m_allocator.deallocate(node, 1);
                return iterator ( NULL, 0 );
            } else if ( node == m_head && node != m_tail ) {
                node_type *ret = node->m_iter_next;
                node->m_iter_next->m_iter_prev = NULL;
                m_head = node->m_iter_next;
                m_allocator.destroy(node);
                m_allocator.deallocate(node, 1);
                return iterator ( ret, 0 );
            } else {
                node_type *tmp = m_head;
                m_head = m_tail = NULL;
                m_allocator.destroy(tmp);
                m_allocator.deallocate(tmp, 1);
                return iterator ( NULL, 0 );
            }
        }
    }
    
    reverse_iterator remove ( reverse_iterator i ) {
        if ( !m_count ) {
            return reverse_iterator ( NULL, 0 );
        } else {
            node_type *node = i.m_ptr;
            m_count--;
            
            if ( node != m_head && node != m_tail ) {
                node_type *ret = node->m_iter_prev;
                node->m_iter_prev->m_iter_next = node->m_iter_next;
                node->m_iter_next->m_iter_prev = node->m_iter_prev;
                m_allocator.destroy(node);
                m_allocator.deallocate(node, 1);
                return reverse_iterator ( ret, NULL );
            } else if ( node != m_head && node == m_tail ) {
                node_type *ret = node->m_iter_prev;
                node->m_iter_prev->m_iter_next = NULL;
                m_tail = node->m_iter_prev;
                m_allocator.destroy(node);
                m_allocator.deallocate(node, 1);
                return reverse_iterator ( ret, 0 );
            } else if ( node == m_head && node != m_tail ) {
                node->m_iter_next->m_iter_prev = NULL;
                m_head = node->m_iter_next;
                m_allocator.destroy(node);
                m_allocator.deallocate(node, 1);
                return reverse_iterator ( NULL, 0 );
            } else {
                node_type *tmp = m_head;
                m_head = m_tail = NULL;
                m_allocator.destroy(tmp);
                m_allocator.deallocate(tmp, 1);
                return reverse_iterator ( NULL, 0 );
            }
        }
    }
    iterator begin() {
        return iterator ( m_head, 0 );
    }
    iterator end() {
        return iterator ( NULL, 0 );
    }
    const_iterator begin() const {
        return const_iterator ( m_head, 0 );
    }
    const_iterator end() const {
        return const_iterator ( NULL, 0 );
    }
	reverse_iterator rbegin() {
		return reverse_iterator ( m_tail, 0 );
	}
	reverse_iterator rend() {
		return reverse_iterator ( NULL, 0 );
	}
	const_reverse_iterator rbegin() const {
		return const_reverse_iterator ( m_tail, 0 );
	}
	const_reverse_iterator rend() const {
		return const_reverse_iterator ( NULL, 0 );
	}
    euint size() const {
        return m_count;
    }
    list<T>& operator =(const list<T>& rts) {
        clear();
        const_iterator iter = rts.begin();
        const_iterator iter_end = rts.end();
        for (; iter != iter_end; iter++) {
            push_back(*iter);
        }
        return *this;
    }
    T& back() {
        return m_tail->m_value;
    }
    T& front() {
        return m_head->m_value;
    }
    void pop_front() {
        if (m_head) {
            node_type* tmp = m_head;
            m_head = m_head->m_iter_next;
            if (!m_head) {
                m_tail = NULL;
            }
            m_allocator.destroy(tmp);
            m_allocator.deallocate(tmp, 1);
            m_count--;
        }
    }
    void pop_back() {
        if (m_tail) {
            node_type* tmp = m_tail;
            m_tail = m_head->m_iter_prev;
            if (!m_tail) {
                m_head = NULL;
            }
            m_allocator.destroy(tmp);
            m_allocator.deallocate(tmp, 1);
            m_count--;
        }
    }
	void throw_front(iterator i) {
		if (!m_count)
			return;
		{
			node_type* node = i.m_ptr;

			if (node != m_head && node != m_tail)
			{
				node->m_iter_prev->m_iter_next = node->m_iter_next;
				node->m_iter_next->m_iter_prev = node->m_iter_prev;
			}
			else if (node != m_head && node == m_tail)
			{
				node->m_iter_prev->m_iter_next = NULL;
				m_tail = node->m_iter_prev;
			}
			else
			{
				return;
			}

			m_head->m_iter_prev = node;
			node->m_iter_next = m_head;
			node->m_iter_prev = NULL;
			m_head = node;
		}
	}
	void throw_back(iterator i) {
		if (!m_count)
			return;
		{
			node_type* node = i.m_ptr;

			if (node != m_head && node != m_tail)
			{
				node->m_iter_prev->m_iter_next = node->m_iter_next;
				node->m_iter_next->m_iter_prev = node->m_iter_prev;
			}
			else if (node == m_head && node != m_tail)
			{
				node->m_iter_next->m_iter_prev = NULL;
				m_head = node->m_iter_next;
			}
			else
			{
				return;
			}

			m_tail->m_iter_next = node;
			node->m_iter_prev = m_tail;
			node->m_iter_next = NULL;
			m_tail = node;
		}
	}
};

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
