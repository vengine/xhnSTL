/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_FIXED_QUEUE_HPP
#define XHN_FIXED_QUEUE_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
namespace xhn
{
template <typename T>
class fixed_queue
{
private:
	T* m_queue;
	T* m_top_barrier;
	T* m_bottom_barrier;
	T* m_top_pointer;
	T* m_bottom_pointer;
public:
    class iterator
	{
		friend class fixed_queue;
	private:
        T* m_ptr;
		T* m_top_barrier;
		T* m_bottom_barrier;
		iterator(T* ptr, T* top_barrier, T* bottom_barrier)
			: m_ptr(ptr)
			, m_top_barrier(top_barrier)
			, m_bottom_barrier(bottom_barrier)
		{}
	public:
		inline iterator(const iterator& iter)
			: m_ptr(iter.m_ptr)
			, m_top_barrier(iter.m_top_barrier)
			, m_bottom_barrier(iter.m_bottom_barrier)
		{}
		inline iterator& operator = (iterator& iter) {
			m_ptr = iter.m_ptr;
			m_top_barrier = iter.m_top_barrier;
			m_bottom_barrier = iter.m_bottom_barrier;
		}
		inline bool operator == (iterator& iter) {
			return m_ptr == iter.m_ptr;
		}
		inline bool operator != (iterator& iter) {
			return m_ptr != iter.m_ptr;
		}
		inline iterator &operator++() {
			m_ptr++;
			if (m_ptr == m_bottom_barrier)
				m_ptr = m_top_barrier;
			return *this;
		}
		inline iterator operator++ ( int ) {
			iterator tmp = *this;
			++*this;
			return tmp;
		}
		inline T& operator*() {
			return *m_ptr;
		}
	};
public:
	fixed_queue(euint size) {
		m_queue = new T[size];
		m_top_barrier = m_queue;
		m_bottom_barrier = m_queue + size;
		m_top_pointer = m_queue;
		m_bottom_pointer = m_top_pointer;
	}
	~fixed_queue() {
        delete[] m_queue;
	}
	bool push(const T& from) {
		T* registed_bottom_pointer = m_bottom_pointer;
		registed_bottom_pointer++;
		if (registed_bottom_pointer == m_bottom_barrier) {
			registed_bottom_pointer = m_top_barrier;
		}
		if (registed_bottom_pointer == m_top_pointer)
			return false;
		*m_bottom_pointer = from;
		m_bottom_pointer = registed_bottom_pointer;
		return true;
	}
	bool pop(T& to) {
		if (m_top_pointer == m_bottom_pointer)
			return false;
		to = *m_top_pointer;
        m_top_pointer++;
		if (m_top_pointer == m_bottom_barrier) {
			m_top_pointer = m_top_barrier;
		}
		return true;
	}
	bool pop() {
		if (m_top_pointer == m_bottom_pointer)
			return false;
		m_top_pointer++;
		if (m_top_pointer == m_bottom_barrier) {
			m_top_pointer = m_top_barrier;
		}
		return true;
	}
    bool empty() const {
        return m_top_pointer == m_bottom_pointer;
    }
	iterator begin() {
		return iterator(m_top_pointer, m_top_barrier, m_bottom_barrier);
	}
	iterator end() {
		return iterator(m_bottom_pointer, m_top_barrier, m_bottom_barrier);
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
