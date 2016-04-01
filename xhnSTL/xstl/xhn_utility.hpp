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
    
typedef decltype(nullptr) nullptr_t;

/// 内部常数
template <typename T, T V>
struct internal_constant
{
    static constexpr const T      value = V;
    typedef T               value_type;
    typedef internal_constant type;
    constexpr operator value_type() const noexcept {return value;}
};

template <typename T, T V>
constexpr const T internal_constant<T, V>::value;

/// 内部常数的布尔实例
typedef internal_constant<bool, true>  true_constant;
typedef internal_constant<bool, false> false_constant;

/// 将一个类型转为右值引用
template <typename T> struct add_rvalue_reference                      {typedef T&& type;};
template <>          struct add_rvalue_reference<void>                {typedef void type;};
template <>          struct add_rvalue_reference<const void>          {typedef const void type;};
template <>          struct add_rvalue_reference<volatile void>       {typedef volatile void type;};
template <>          struct add_rvalue_reference<const volatile void> {typedef const volatile void type;};

template <typename T>
typename add_rvalue_reference<T>::type
declval() noexcept;

/// 将一个引用转为指针

template <typename T>
inline
T*
addressof(T& value) noexcept
{
    return (T*)&reinterpret_cast<const volatile char&>(value);
}

// 如果是const，则返回原始类型

template <typename T> struct remove_const            {typedef T type;};
template <typename T> struct remove_const<const T>   {typedef T type;};


// 如果是volatile，则返回原始类型

template <typename T> struct remove_volatile             {typedef T type;};
template <typename T> struct remove_volatile<volatile T> {typedef T type;};


// remove_const + remove_volatile

template <typename T> struct remove_cv
{typedef typename remove_volatile<typename remove_const<T>::type>::type type;};


// 判断是否是void

template <typename T> struct is_void_impl       : public false_constant {};
template <>          struct is_void_impl<void> : public true_constant {};

template <typename T> struct is_void
: public is_void_impl<typename remove_cv<T>::type> {};
    

template <typename T> struct is_pointer_impl    : public false_constant {};
template <typename T> struct is_pointer_impl<T*> : public true_constant {};
    
template <typename T> struct is_pointer
: public is_pointer_impl<typename remove_cv<T>::type> {};

// 判断是否是nullptr

template <typename T> struct is_nullptr_t_impl       : public false_constant {};
template <>          struct is_nullptr_t_impl<nullptr_t> : public true_constant {};

template <typename T> struct is_nullptr_t
: public is_nullptr_t_impl<typename remove_cv<T>::type> {};

/// 判断是否是联合

template <typename T> struct is_union_impl : public false_constant {};
template <typename T> struct is_union
: public is_union_impl<typename remove_cv<T>::type> {};

/// 判断是否是class

namespace detail {
    template <typename T> char test(int T::*);
    struct two_chars { char c[2]; };
    template <typename T> two_chars test(...);
}

template <typename T>
struct is_class : internal_constant<bool, sizeof(detail::test<T>(0))==1
&& !is_union<T>::value> {};

/// 判断是否是数组

template <typename T> struct is_array                 : public false_constant {};
template <typename T> struct is_array<T[]>            : public true_constant {};
template <typename T, euint N> struct is_array<T[N]> : public true_constant {};

/// 如果是数组，则返回类型
template <typename T> struct remove_extent                {typedef T type;};
template <typename T> struct remove_extent<T[]>           {typedef T type;};
template <typename T, euint N> struct remove_extent<T[N]> {typedef T type;};

/// 判断是否是引用

template <typename T> struct is_lvalue_reference       : public false_constant {};
template <typename T> struct is_lvalue_reference<T&>   : public true_constant {};

template <typename T> struct is_rvalue_reference       : public false_constant {};

template <typename T> struct is_reference              : public false_constant {};
template <typename T> struct is_reference<T&>          : public true_constant {};

/// 如果类型是引用（左值或右值皆可），返回的是引用前的类型
template <typename T> struct remove_reference      {typedef T type;};
template <typename T> struct remove_reference<T&>  {typedef T type;};
template <typename T> struct remove_reference<T&&> {typedef T type;};
    
/// 如果类型是指针，返回指针的类型
template<typename T> struct remove_pointer                    {typedef T type;};
template<typename T> struct remove_pointer<T*>                {typedef T type;};
template<typename T> struct remove_pointer<T* const>          {typedef T type;};
template<typename T> struct remove_pointer<T* volatile>       {typedef T type;};
template<typename T> struct remove_pointer<T* const volatile> {typedef T type;};

/// 判断两个类型是否相同

template <typename T, typename U> struct is_same       : public false_constant {};
template <typename T>          struct is_same<T, T> : public true_constant {};

template <typename _Bp, typename _Dp>
struct is_base_of
: public internal_constant<bool, __is_base_of(_Bp, _Dp)> {};

/// 获取一个类型的指针类型

template <typename T> struct add_pointer
{typedef typename remove_reference<T>::type* type;};


/// 判断是否是函数

// primary template
template<typename>
struct is_function : false_constant { };

// specialization for regular functions
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)> : true_constant {};

// specialization for variadic functions such as std::printf
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)> : true_constant {};

// specialization for function types that have cv-qualifiers
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)volatile> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const volatile> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)const> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)volatile> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)const volatile> : true_constant {};

// specialization for function types that have ref-qualifiers
template<typename Ret, typename... Args>
struct is_function<Ret(Args...) &> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const &> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)volatile &> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const volatile &> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...) &> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)const &> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)volatile &> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)const volatile &> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...) &&> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const &&> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)volatile &&> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const volatile &&> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...) &&> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)const &&> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)volatile &&> : true_constant {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...,...)const volatile &&> : true_constant {};

/// 判断是否是成员，成员包含成员变量和成员函数
    
template< typename T >
struct is_member_pointer_helper         : false_constant {};
    
template< typename T, typename U >
struct is_member_pointer_helper<T U::*> : true_constant {};
    
template< typename T >
struct is_member_pointer :
is_member_pointer_helper<typename remove_cv<T>::type> {};
    
/// 判断是否是成员函数
    
template< typename T >
struct is_member_function_pointer_helper : false_constant {};
    
template< typename T, typename U>
struct is_member_function_pointer_helper<T U::*> : is_function<T> {};
    
template< typename T >
struct is_member_function_pointer : is_member_function_pointer_helper<
typename remove_cv<T>::type
> {};
    
/// 判断是否是成员变量
    
template<typename T>
struct is_member_object_pointer : internal_constant<
bool,
is_member_pointer<T>::value &&
!is_member_function_pointer<T>::value
> {};

/// xhn::move(t) 负责将表达式 t 转换为右值，使用这一转换意味着你不再关心 t 的内容，它可以通过被移动（窃取）来解决移动语意问题
template <typename T>
inline typename remove_reference<T>::type&&
move(T&& t) noexcept
{
    typedef typename remove_reference<T>::type UpType;
    return static_cast<UpType&&>(t);
}

/// xhn::forward<T>(u) 有两个参数：T 与 u。当T为左值引用类型时，u将被转换为T类型的左值，否则u将被转换为T类型右值。
/// 如此定义xhn::forward是为了在使用右值引用参数的函数模板中解决参数的完美转发问题。
template<typename T>
inline T&&
forward(typename remove_reference<T>::type& t)
{ return static_cast<T&&>(t); }

template<typename T>
inline T&&
forward(typename remove_reference<T>::type&& t)
{
    static_assert(!is_lvalue_reference<T>::value,
                  "template argument substituting T is an lvalue reference type");
    return static_cast<T&&>(t);
}

//////////////////////////////////////////////////////////
namespace detail {
    template <typename F, typename... Args>
    inline auto INVOKE(F&& f, Args&&... args) ->
    decltype(forward<F>(f)(forward<Args>(args)...)) {
        return forward<F>(f)(forward<Args>(args)...);
    }
    
    template <typename Base, typename T, typename Derived>
    inline auto INVOKE(T Base::*pmd, Derived&& ref) ->
    decltype(forward<Derived>(ref).*pmd) {
        return forward<Derived>(ref).*pmd;
    }
    
    template <typename PMD, typename Pointer>
    inline auto INVOKE(PMD&& pmd, Pointer&& ptr) ->
    decltype((*forward<Pointer>(ptr)).*forward<PMD>(pmd)) {
        return (*forward<Pointer>(ptr)).*forward<PMD>(pmd);
    }
    
    template <typename Base, typename T, typename Derived, typename... Args>
    inline auto INVOKE(T Base::*pmf, Derived&& ref, Args&&... args) ->
    decltype((forward<Derived>(ref).*pmf)(forward<Args>(args)...)) {
        return (forward<Derived>(ref).*pmf)(forward<Args>(args)...);
    }
    
    template <typename PMF, typename Pointer, typename... Args>
    inline auto INVOKE(PMF&& pmf, Pointer&& ptr, Args&&... args) ->
    decltype(((*forward<Pointer>(ptr)).*forward<PMF>(pmf))(forward<Args>(args)...)) {
        return ((*forward<Pointer>(ptr)).*forward<PMF>(pmf))(forward<Args>(args)...);
    }
} // namespace detail

// Minimal C++11 implementation:
template <typename> struct result_of;
template <typename F, typename... ArgTypes>
struct result_of<F(ArgTypes...)> {
    using type = decltype(detail::INVOKE(declval<F>(), declval<ArgTypes>()...));
};

template< typename F, typename... ArgTypes>
result_of<F&&(ArgTypes&&...)> invoke(F&& f, ArgTypes&&... args) {
    return detail::INVOKE(forward<F>(f), forward<ArgTypes>(args)...);
}
//////////////////////////////////////////////////////////

/// 引用包裹，将一个引用包裹为一个可拷贝可赋值得物体

template <typename T>
class reference_wrapper {
public:
    // types
    typedef T type;
    
    // construct/copy/destroy
    reference_wrapper(T& ref) noexcept : _ptr(addressof(ref)) {}
    reference_wrapper(T&&) = delete;
    reference_wrapper(const reference_wrapper&) noexcept = default;
    
    // assignment
    reference_wrapper& operator=(const reference_wrapper& x) noexcept = default;
    
    // access
    operator T& () const noexcept { return *_ptr; }
    T& get() const noexcept { return *_ptr; }
    
    template< typename... ArgTypes >
    typename result_of<T&(ArgTypes&&...)>::type
    operator() ( ArgTypes&&... args ) const {
        return invoke(get(), forward<ArgTypes>(args)...);
    }
    
private:
    T* _ptr;
};

//////////////////////////////////////////////////////////
template<bool B, typename T, typename F>
struct conditional { typedef T type; };

template<typename T, typename F>
struct conditional<false, T, F> { typedef F type; };
//////////////////////////////////////////////////////////
template< typename T >
struct decay {
private:
    typedef typename remove_reference<T>::type U;
public:
    typedef typename conditional<is_array<U>::value,
    typename remove_extent<U>::type*,
    typename conditional<
    is_function<U>::value,
    typename add_pointer<U>::type,
    typename remove_cv<U>::type
    >::type
    >::type type;
};
    
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

template<typename T>
struct FLessThanProc
{
	bool operator()(const T& a, const T& b) const {
		return a < b;
	}
};
    
template<typename T>
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

	template<typename A>
	allocator(const allocator<A>&)
	{
	}

	template<typename A>
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
