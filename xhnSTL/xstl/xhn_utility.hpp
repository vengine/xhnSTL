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
#include <math.h>
#include <algorithm>

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
    return calc_hashnr ( key, strlen ( key ) );
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
    void operator () (T* dst, T&& src) {
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
#elif defined(LINUX)
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
#elif defined(LINUX)
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
    
    struct FHashCharPointer
    {
        euint32 operator ()(const char* key) const {
            return calc_hashnr ( key, _strlen ( key ) );
        }
    };
    struct FHashWCharPointer
    {
        euint32 operator ()(const wchar_t* key) const {
            int count = 0;
            while (key[count]) {
                count++;
            }
            return calc_hashnr ( (const char*)key, count * sizeof(wchar_t) );
        }
    };
    struct FHashVoidPointer
    {
        euint32 operator ()(const void* key) const {
            return calc_hashnr ( (const char*)&key, sizeof(void*) );
        }
    };
    template <typename T>
    struct FHashSpec
    {
        euint32 operator ()(const T& key) const {
            return key.hash_value();
        }
    };
    template <typename T>
    struct FHashImme
    {
        euint32 operator ()(const T& key) const {
            return (euint32)key;
        }
    };
    
    template<typename T>
    struct HasHashValueMethod
    {
        template<typename U, euint32 (U::*)() const> struct SFINAE {};
        template<typename U> static char Test(SFINAE<U, &U::hash_value>*);
        template<typename U> static int Test(...);
        static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
    };
    
    struct FUpdateCharPointerHashStatusProc
    {
        void operator ()(::hash_calc_status* status, const char* key) const {
            ::update_hash_status( status, key, _strlen ( key ) );
        }
    };
    
    struct FUpdateWCharPointerHashStatusProc
    {
        void operator ()(::hash_calc_status* status, const wchar_t* key) const {
            int count = 0;
            while (key[count]) {
                count++;
            }
            ::update_hash_status ( status, (const char*)key, count * sizeof(wchar_t) );
        }
    };
    
    struct FUpdateVoidPointerHashStatusProc
    {
        void operator ()(::hash_calc_status* status, const void* key) const {
            ::update_hash_status( status, (const char*)&key, sizeof(void*) );
        }
    };
    
    template <typename T>
    struct FUpdateHashStatusSpecProc
    {
        void operator ()(struct hash_calc_status* status, const T& key) const {
            key.update_hash_status(status);
        }
    };
    
    template <typename T>
    struct FUpdateHashStatusImmeProc
    {
        void operator ()(::hash_calc_status* status, const T& key) const {
            ::update_hash_status( status, (const char*)&key, sizeof(key) );
        }
    };
    
    template <typename T>
    struct FUpdateHashStatusProc
    {
        typedef
        typename conditional<is_pointer<T>::value,
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, char>::value, FUpdateCharPointerHashStatusProc,
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, wchar_t>::value, FUpdateWCharPointerHashStatusProc, FUpdateVoidPointerHashStatusProc>::type
        >::type,
        typename conditional<HasHashValueMethod<T>::Has, FUpdateHashStatusSpecProc<T>, FUpdateHashStatusImmeProc<T>>::type
        >::type updateHashStatusProc;
        updateHashStatusProc proc;
        void operator() (struct hash_calc_status* status, const T& v) const {
            proc ( status, v );
        }
    };
    
    template <typename T>
    struct FHashProc
    {
        typedef
        typename conditional<is_pointer<T>::value,
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, char>::value, FHashCharPointer,
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, wchar_t>::value, FHashWCharPointer, FHashVoidPointer>::type
        >::type,
        typename conditional<HasHashValueMethod<T>::Has, FHashSpec<T>, FHashImme<T>>::type
        >::type hashProc;
        hashProc proc;
        euint32 operator() (const T& v) const {
            return proc ( v );
        }
    };
    
    struct FEqualCharPointer
    {
        bool operator ()(const char* a, const char* b) const {
            return strcmp(a, b);
        }
    };
    struct FEqualWCharPointer
    {
        bool operator ()(const wchar_t* a, const wchar_t* b) const {
            int count = 0;
            while (a[count] && b[count]) {
                if (a[count] != b[count])
                    return false;
                count++;
            }
            if (!a[count] && !b[count])
                return true;
            else
                return false;
        }
    };
    struct FEqualVoidPointer
    {
        bool operator ()(const void* a, const void* b) const {
            return a == b;
        }
    };
    template <typename T>
    struct FEqualSpec
    {
        bool operator ()(const T& a, const T& b) const {
            return a.equal(b);
        }
    };
    template <typename T>
    struct FEqualImme
    {
        bool operator ()(const T& a, const T& b) const {
            return a == b;
        }
    };
    
    template<typename T>
    struct HasEqualValueMethod
    {
        template<typename U, bool (U::*)() const> struct SFINAE {};
        template<typename U> static char Test(SFINAE<U, &U::equal>*);
        template<typename U> static int Test(...);
        static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
    };
    template <typename T>
    struct FEqualProc
    {
        typedef
        typename conditional<is_pointer<T>::value,
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, char>::value, FEqualCharPointer,
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, wchar_t>::value, FEqualWCharPointer, FEqualVoidPointer>::type
        >::type,
        typename conditional<HasEqualValueMethod<T>::Has, FEqualSpec<T>, FEqualImme<T>>::type
        >::type equalProc;
        equalProc proc;
        bool operator() (const T& a, const T& b) const {
            return proc ( a, b );
        }
    };
    
    struct FStrLenProc {
        size_t operator() (const char* str) const {
            return strlen(str);
        }
    };
    
    struct FStrCmpProc {
        int operator() (const char* str0, const char* str1) const {
            return strcmp(str0, str1);
        }
    };
    struct FDefaultStrProc {
        const char* operator() () const {
            return "";
        }
    };
    
    struct FWStrLenProc {
        size_t operator() (const wchar_t* str) const {
            size_t ret = 0;
            while (str[ret]) {
                ++ret;
            }
            return ret;
        }
    };
    
    struct FWStrCmpProc {
        int operator() (const wchar_t* str0, const wchar_t* str1) const {
            int count = 0;
            while (str0[count]) {
                if (!str1[count])
                    return 1;
                if (str0[count] < str1[count])
                    return -1;
                else if (str0[count] > str1[count])
                    return 1;
                count++;
            }
            if (str1[count])
                return -1;
            return 0;
        }
    };
    struct FDefaultWStrProc {
        const wchar_t* operator() () const {
            return L"";
        }
    };
    
#if USING_STRING_OLD
    template <typename T, unsigned NUM_EXTENDED_WORDS, typename STR_LEN_PROC, typename STR_CMP_PROC, typename DEFAULT_STR_PROC> class string_base_old;
#else
    template <typename T, unsigned NUM_EXTENDED_WORDS, typename STR_LEN_PROC, typename STR_CMP_PROC, typename DEFAULT_STR_PROC> class string_base;
#endif
    
    template <typename T>
    struct TKeyValue
    {
        typedef
        typename conditional<is_pointer<T>::value,
#if USING_STRING_OLD
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, char>::value, string_base_old<char, 1, FStrLenProc, FStrCmpProc, FDefaultStrProc>,
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, wchar_t>::value, string_base_old<wchar_t, 1, FWStrLenProc, FWStrCmpProc, FDefaultWStrProc>, void*>::type
#else
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, char>::value, string_base<char, 1, FStrLenProc, FStrCmpProc, FDefaultStrProc>,
        typename conditional<is_same<typename remove_cv<typename remove_pointer<T>::type>::type, wchar_t>::value, string_base<wchar_t, 1, FWStrLenProc, FWStrCmpProc, FDefaultWStrProc>, void*>::type
#endif
        >::type,
        T
        >::type type;
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
    
template <typename T>
void swap(T& x, T& y) noexcept
{
    T t(move(x));
    x = move(y);
    y = move(t);
}
    
inline int med3(int x[], int a, int b, int c) {
    return x[a] < x[b] ? (x[b] < x[c] ? b : x[a] < x[c] ? c : a)
    : x[b] > x[c] ? b : x[a] > x[c] ? c : a;
}
    
inline void swap(int* a, int i, int j) {
    int temp = a[i];
    a[i] = a[j];
    a[j] = temp;
}
    
inline void vecswap(int x[], int a, int b, int n) {
    for (int i = 0; i < n; i++, a++, b++)
        swap(x, a, b);
}
    
template <typename RandomAccessIterator, typename T>
void nth_element_impl(RandomAccessIterator first, RandomAccessIterator nth,
                      RandomAccessIterator last) {
    if (first != nth)
    {
        T tmp = *nth;
        *nth = *first;
        *first = tmp;
    }
    RandomAccessIterator i = first;
    RandomAccessIterator j = last - 1;
    T x = *first;
    while (i < j)
    {
        while(i < j && *j >= x)
            j--;
        if(i < j) {
            *i = *j;
            i++;
        }
        while(i < j && *i < x)
            i++;
        if(i < j) {
            *j = *i;
            j--;
        }
    }
    *i = x;
}
    
template <typename RandomAccessIterator>
void nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                 RandomAccessIterator last) {
    std::nth_element<RandomAccessIterator>(first, nth, last);
}
    
template <typename RandomAccessIterator, typename Compare>
void nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                 RandomAccessIterator last) {
    std::nth_element<RandomAccessIterator, Compare>(first, nth, last);
}
    
/// CALC_MID_PROC(min, max)
/// GREATER_THAN_SET(a, b, c, d) return a > b ? c : d
/// LESS_THAN_SET(a, b, c, d)    return a < b ? c : d
/// LESS_THAN_SET2(a, b, c0, c1, c2, d0, d1, d2, r0, r1, r2)
/// r0 = a < b ? c0 : d0
/// r1 = a < b ? c1 : d1
/// r2 = a < b ? c2 : d2
    
template <typename VARRAY, typename VALUE, typename INDEX,
          typename INIT_MAX_PROC, typename INIT_MIN_PROC, typename INIT_MID_INDEX_PROC,
          typename ABS_PROC, typename CALC_MID_PROC,
          typename GREATER_THAN_SET_PROC, typename LESS_THAN_SET_PROC,
          typename LESS_PROC, typename INC_PROC,
          typename LESS_THAN_SET2_PROC>
void calc_limit(const VARRAY& array,
                VALUE& min, VALUE& max, VALUE& mid,
                INDEX& mid_index,
                INDEX first, INDEX last)
{
    INIT_MAX_PROC init_max;
    INIT_MIN_PROC init_min;
    INIT_MID_INDEX_PROC init_mid_index;
    ABS_PROC abs;
    CALC_MID_PROC calc_mid;
    GREATER_THAN_SET_PROC greater_than_set;
    LESS_THAN_SET_PROC less_than_set;
    LESS_PROC less;
    INC_PROC inc;
    LESS_THAN_SET2_PROC less_than_set2;
    
    min = init_min();
    max = init_max();
    mid = init_min();
    VALUE mid_dist = init_min();
    mid_index = init_mid_index();
    euint counter = 0;
    for (INDEX i = first; less(i, last); inc(i), counter++) {
        VALUE t = array[i];
        max = greater_than_set(t, max, t, max);
        min = less_than_set(t, min, t, min);
        if (counter >= 2) {
            VALUE curt_mid = calc_mid(min, max);
            VALUE curt_dist = abs(t - curt_mid);
            euint curt_index = i;
            less_than_set2(curt_dist, mid_dist,
                           t, curt_dist, curt_index,
                           mid, mid_dist, mid_index,
                           mid, mid_dist, mid_index);
        }
    }
}
    
struct FInitMaxFloat
{
    float operator () () const {
        return -FLT_MAX;
    }
};
    
struct FInitMinFloat
{
    float operator () () const {
        return FLT_MAX;
    }
};
    
struct FInitMidIndexEUint
{
    euint operator () () const {
        return 0;
    }
};
    
struct FAbsFloat
{
    float operator() (float a) const {
        return fabsf(a);
    }
};
    
struct FCalcMidFloat
{
    float operator () (float min, float max) const {
        return 0.5f * (max + min);
    }
};
    
struct FGreaterThanSetFloat
{
    float operator () (float a, float b, float c, float d) const {
        if (a > b) {
            return c;
        }
        else {
            return d;
        }
    }
};
    
struct FLessThanSetFloat
{
    float operator () (float a, float b, float c, float d) const {
        if (a < b) {
            return c;
        }
        else {
            return d;
        }
    }
};
    
struct FLessProcEUint
{
    bool operator () (euint a, euint b) const {
        return a < b;
    }
};
    
struct FIncProcEUint
{
    void operator () (euint& a) const {
        a++;
    }
};
    
struct FLessThanSet2FloatEUint
{
    void operator () (float a, float b,
                      float c0, float c1, euint c2,
                      float d0, float d1, euint d2,
                      float& r0, float& r1, euint& r2) const {
        if (a < b) {
            r0 = c0;
            r1 = c1;
            r2 = c2;
        }
        else {
            r0 = d0;
            r1 = d1;
            r2 = d2;
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
