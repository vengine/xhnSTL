/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_TUPLE_HPP
#define XHN_TUPLE_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"

#include "xhn_utility.hpp"

namespace xhn
{
/// 有两种tuple，有模板参数的tuple和没有模板参数的tuple
template<typename... TList> struct tuple;
template<> struct tuple<> {};
/// 没有模板参数的tuple的别名是empty_tuple
typedef tuple<> empty_tuple;

//////////////////////////////////////////////////////////
/// 多一个T参数的tuple继承自少一个T参数的tuple，递归继承的方式
template<typename T, typename... TList>
struct tuple<T, TList...> : public tuple<TList...>
{
protected:
    ALIGN_FLAG ( 16 ) T m_value;
protected:
    typedef T value_type;
    typedef tuple<TList...> base_type;
    typedef tuple<T, TList...> this_type;
public:
    tuple(const T& v, const TList&... tails):base_type(tails...),m_value(v) {}
    
    tuple(T&& v, TList&&... tails):base_type(move(tails)...), m_value(forward<T>(v)) {}
    tuple(T&& v, TList&... tails):base_type(move(tails)...), m_value(forward<T>(v)) {}
    tuple(T& v, TList&&... tails):base_type(move(tails)...), m_value(forward<T>(v)) {}
    
    tuple(const this_type& other):base_type(static_cast<const base_type&>(other)),m_value(other.m_value)
    {}
    
    tuple(this_type&& other):base_type(move(static_cast<base_type&>(other))),m_value(forward<T>(other.m_value))
    {}
    
    const T& head() const { return this->m_value; }
    T& head() { return this->m_value; }
    
    this_type& operator=(const this_type& other)
    {
        base_type::operator=(static_cast<const base_type&>(other));
        m_value = other.m_value;
        return *this;
    }
    
    this_type& operator=(this_type&& other)
    {
        base_type::operator=(move(static_cast<base_type&>(other)));
        m_value = other.m_value;
        return *this;
    }
    
    bool operator<(const this_type& other) const
    {
        return
        base_type::operator<(move(static_cast<const base_type&>(other)))
        && (m_value < other.m_value);
    }
    bool operator<(const this_type&& other) const
    {
        return
        base_type::operator<(move(static_cast<const base_type&>(other)))
        && (m_value < other.m_value);
    }
};
/// 只有一个T参数的tuple继承自空参数的empty_tuple
template<typename T>
struct tuple<T> : public empty_tuple
{
protected:
    ALIGN_FLAG ( 16 ) T m_value;
protected:
    typedef T value_type;
    typedef empty_tuple base_type;
    typedef tuple<T> this_type;
public:
    tuple(const T& v):m_value(v) {}
    tuple(T&& v):m_value(forward<T>(v)) {}
    
    tuple(const this_type& other):m_value(other.m_value) {}
    
    tuple(this_type&& other):m_value(forward<T>(other.m_value)) {}
    
    const T& head() const { return this->m_value; }
    T& head() { return this->m_value; }
    this_type& operator=(const this_type& other)
    {
        m_value = other.m_value;
        return *this;
    }
    this_type& operator=(this_type&& other)
    {
        m_value = other.m_value;
        return *this;
    }
    bool operator<(const this_type& other) const
    {
        return m_value < other.m_value;
    }
    bool operator<(const this_type&& other) const
    {
        return m_value < other.m_value;
    }
};

//////////////////////////////////////////////////////////
template<unsigned int N, typename... TList> struct tuple_element;

template<unsigned int N, typename T, typename... TList>
struct tuple_element< N, tuple<T, TList...> >
{
    typedef typename tuple_element< N-1, tuple<TList...> >::value_type value_type;
    typedef typename tuple_element< N-1, tuple<TList...> >::tuple_type tuple_type;
};

template<typename T, typename... TList>
struct tuple_element< 0, tuple<T, TList...> >
{
    typedef T value_type;
    typedef tuple<T, TList...> tuple_type;
};

template<>
struct tuple_element<0, empty_tuple>
{
    typedef empty_tuple value_type;
    typedef empty_tuple tuple_type;
};

//////////////////////////////////////////////////////////
template<unsigned int N, typename... TList>
constexpr const typename tuple_element<N, tuple<TList...> >::value_type&
get(const tuple<TList...>& tuple_)
{
    typedef tuple<TList...> tuple_type;
    typedef typename tuple_element<N, tuple_type>::tuple_type base_tuple_type;
    
    return static_cast<const base_tuple_type&>(tuple_).head();
}

template<unsigned int N, typename... TList>
typename tuple_element<N, tuple<TList...> >::value_type&
get(tuple<TList...>& tuple_)
{
    typedef tuple<TList...> tuple_type;
    typedef typename tuple_element<N, tuple_type>::tuple_type base_tuple_type;
    
    return static_cast<base_tuple_type&>(tuple_).head();
}

template <class T>
struct unwrap_refwrapper
{
    using type = T;
};

template <class T>
struct unwrap_refwrapper<reference_wrapper<T>>
{
    using type = T&;
};

template <class T>
using special_decay_t = typename unwrap_refwrapper<typename decay<T>::type>::type;

template <class... Types>
tuple<special_decay_t<Types>...> make_tuple(Types&&... args)
{
    return tuple<special_decay_t<Types>...>(forward<Types>(args)...);
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
