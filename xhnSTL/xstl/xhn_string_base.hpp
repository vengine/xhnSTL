/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_STRING_BASE_HPP
#define XHN_STRING_BASE_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "estring.h"
#include "xhn_utility.hpp"
#include "xhn_vector.hpp"
#include <string.h>
namespace xhn
{
template <typename C, unsigned NUM_EXTENDED_WORDS>
struct string_data0
{
    euint m_size;
    C *m_str;
    euint m_extendedWords[NUM_EXTENDED_WORDS];
};
    
#define DATA1_SIZE (sizeof(string_data0<C, NUM_EXTENDED_WORDS>) / sizeof(C) - 1)
template <typename C, unsigned NUM_EXTENDED_WORDS>
struct string_data1
{
    C m_str[DATA1_SIZE];
    C m_flag;
};
    
    template <typename C, unsigned NUM_EXTENDED_WORDS, typename STR_LEN_PROC, typename STR_CMP_PROC, typename DEFAULT_STR_PROC>
    class string_base_old
    {
    private:
        euint m_size;
        C *m_str;
        STR_CMP_PROC m_str_cmp_proc;
        DEFAULT_STR_PROC m_default_str_proc;
        bool m_own_str;
    public:
        const static euint npos = ( euint )-1;
        typedef C* iterator;
        typedef const C* const_iterator;
        string_base_old() {
            m_str = ( C * ) NMalloc ( sizeof(C) );
            m_str[0] = 0;
            m_size = 0;
            m_own_str = true;
        }
        string_base_old ( const C *str, bool own_str = true ) {
            if (!str) {
                str = m_default_str_proc();
                own_str = true;
            }
            
            int count = 0;
            
            while ( str[count] ) {
                count++;
            }
            
            if (own_str) {
                m_str = ( C * ) NMalloc ( (count + 1) * sizeof(C) );
                memcpy ( m_str, str, (count + 1) * sizeof(C) );
            }
            else {
                m_str = ( C * )str;
            }
            m_own_str = own_str;
            m_size = count;
        }
        string_base_old ( const C *str, euint size ) {
            m_str = ( C * ) NMalloc ( (size + 1) * sizeof(C) );
            memcpy ( m_str, str, size * sizeof(C) );
            m_str[size] = 0;
            
            m_own_str = str;
            m_size = size;
        }
        string_base_old ( const vector< C, FGetCharRealSizeProc<C> >& str ) {
            euint count = str.size();
            
            m_str = ( C * ) NMalloc ( (count + 1) * sizeof(C) );
            C* s = str.get();
            memcpy ( m_str, s, count * sizeof(C) );
            m_str[count] = 0;
            m_own_str = true;
            m_size = count;
        }
        string_base_old ( const string_base_old &str ) {
            if (str.m_own_str) {
                m_str = ( C * ) NMalloc ( (str.m_size + 1) * sizeof(C) );
                memcpy ( m_str, str.m_str, (str.m_size + 1) * sizeof(C) );
            }
            else {
                m_str = str.m_str;
            }
            m_own_str = str.m_own_str;
            m_size = str.m_size;
        }
        string_base_old ( string_base_old &&str ) {
            m_str = str.m_str;
            m_own_str = str.m_own_str;
            m_size = str.m_size;
            str.m_own_str = false;
        }
        string_base_old( const string_base_old &str, const char* filename, int line ) {
            if (str.m_own_str) {
                m_str = ( C * ) _Malloc ( (str.m_size + 1) * sizeof(C), filename, line );
                memcpy ( m_str, str.m_str, (str.m_size + 1) * sizeof(C) );
            }
            else {
                m_str = ( C * )str.m_str;
            }
            m_own_str = str.m_own_str;
            m_size = str.m_size;
        }
        ~string_base_old() {
            if (m_own_str) {
                Mfree ( m_str );
            }
        }
        iterator begin() {
            return m_str;
        }
        iterator end() {
            return &m_str[m_size];
        }
        const_iterator begin() const {
            return m_str;
        }
        const_iterator end() const {
            return &m_str[m_size];
        }
        const C *c_str() const {
            return m_str;
        }
        bool operator == ( const C* s ) const {
            return m_str_cmp_proc(m_str, s) == 0;
        }
        bool operator != ( const C* s ) const {
            return m_str_cmp_proc(m_str, s) != 0;
        }
        bool operator < ( const string_base_old &str ) const {
            return m_str_cmp_proc ( m_str, str.m_str ) < 0;
        }
        bool operator == ( const string_base_old &str ) const {
            return m_str_cmp_proc ( m_str, str.m_str ) == 0;
        }
        bool operator != ( const string_base_old &str ) const {
            return m_str_cmp_proc ( m_str, str.m_str ) != 0;
        }
        string_base_old &operator = ( const string_base_old &str ) {
            if (m_own_str) {
                Mfree ( m_str );
            }
            if (str.m_own_str) {
                m_str = ( C * ) NMalloc ( (str.m_size + 1) * sizeof(C) );
                memcpy ( m_str, str.m_str, (str.m_size + 1) * sizeof(C) );
            }
            else {
                m_str = str.m_str;
            }
            m_own_str = str.m_own_str;
            m_size = str.m_size;
            return *this;
        }
        string_base_old &operator = ( string_base_old &&str ) {
            if (m_own_str) {
                Mfree ( m_str );
            }
            m_str = str.m_str;
            m_own_str = str.m_own_str;
            m_size = str.m_size;
            str.m_own_str = false;
            return *this;
        }
        string_base_old &operator = ( const C *str ) {
            euint count = 0;
            
            while ( str[count] ) {
                count++;
            }
            if (m_own_str) {
                Mfree ( m_str );
            }
            m_str = ( C * ) NMalloc ( (count + 1) * sizeof(C) );
            memcpy ( m_str, str, (count + 1) * sizeof(C) );
            m_own_str = true;
            m_size = count;
            return *this;
        }
        string_base_old &operator = ( const vector< C, FGetCharRealSizeProc<C> >& str ) {
            euint count = str.size();
            if (m_own_str) {
                Mfree ( m_str );
            }
            m_str = ( C * ) NMalloc ( (count + 1) * sizeof(C) );
            C* s = str.get();
            memcpy ( m_str, s, count * sizeof(C) );
            m_str[count] = 0;
            m_own_str = true;
            m_size = count;
            return *this;
        }
        string_base_old operator + ( const string_base_old &str ) const {
            string_base_old ret;
            ret.m_size = m_size + str.m_size;
            Mfree ( ret.m_str );
            ret.m_str = ( C * ) NMalloc ( (ret.m_size + 1) * sizeof(C) );
            memcpy ( ret.m_str, m_str, m_size * sizeof(C) );
            memcpy ( &ret.m_str[m_size], str.m_str, (str.m_size + 1) * sizeof(C) );
            return ret;
        }
        string_base_old operator + ( const C *str ) const {
            euint count = 0;
            
            while ( str[count] ) {
                count++;
            }
            
            string_base_old ret;
            ret.m_size = m_size + count;
            Mfree ( ret.m_str );
            ret.m_str = ( C * ) NMalloc ( (ret.m_size + 1) * sizeof(C) );
            memcpy ( ret.m_str, m_str, m_size * sizeof(C) );
            memcpy ( &ret.m_str[m_size], str, (count + 1) * sizeof(C) );
            return ret;
        }
        string_base_old operator + ( C s ) const {
            string_base_old ret;
            ret.m_size = m_size + 1;
            Mfree ( ret.m_str );
            ret.m_str = ( C * ) NMalloc ( (ret.m_size + 1) * sizeof(C) );
            memcpy ( ret.m_str, m_str, m_size * sizeof(C) );
            memcpy ( &ret.m_str[m_size], &s, 2 * sizeof(C) );
            ret.m_hash = _hash ( ret.m_str );
            return ret;
        }
        string_base_old &operator += ( const string_base_old &str ) {
            C *tmp = ( C * ) NMalloc ( (m_size + str.m_size + 1) * sizeof(C) );
            memcpy ( tmp, m_str, m_size * sizeof(C) );
            memcpy ( &tmp[m_size], str.m_str, (str.m_size + 1) * sizeof(C) );
            m_size += str.m_size;
            if (m_own_str) {
                Mfree ( m_str );
            }
            m_own_str = true;
            m_str = tmp;
            return *this;
        }
        string_base_old &operator += ( const C *str ) {
            euint count = 0;
            
            while ( str[count] ) {
                count++;
            }
            
            C *tmp = ( C * ) NMalloc ( (m_size + count + 1) * sizeof(C) );
            memcpy ( tmp, m_str, m_size * sizeof(C) );
            memcpy ( &tmp[m_size], str, (count + 1) * sizeof(C) );
            m_size += count;
            if (m_own_str) {
                Mfree ( m_str );
            }
            m_own_str = true;
            m_str = tmp;
            return *this;
        }
        string_base_old &operator += ( C s ) {
            C *tmp = ( C * ) NMalloc ( (m_size + 2) * sizeof(C) );
            memcpy ( tmp, m_str, m_size * sizeof(C) );
            memcpy ( &tmp[m_size], &s, sizeof(C) );
            tmp[m_size + 1] = 0;
            m_size += 1;
            if (m_own_str) {
                Mfree ( m_str );
            }
            m_own_str = true;
            m_str = tmp;
            return *this;
        }
        string_base_old &operator += ( euint i ) {
            char mbuf[256];
#if BIT_WIDTH == 32
            snprintf(mbuf, 255, "%d", i);
#else
            snprintf(mbuf, 255, "%lld", i);
#endif
            return (operator+=(mbuf));
        }
        euint find ( const string_base_old &str, euint pos = 0 ) const {
            if ( m_size <= pos || !m_size || !str.m_size ) {
                return npos;
            }
            
            euint count = pos;
            euint match_count = 0;
            euint match_pos = pos;
            bool matching = false;
            
            while ( m_str[count] ) {
                if ( matching ) {
                    if ( match_count == str.m_size ) {
                        return match_pos;
                    }
                    
                    if ( m_str[count] != str.m_str[match_count] ) {
                        match_count = 0;
                        matching = false;
                        match_pos = pos + count + 1;
                    }
                    else {
                        
                        match_count++;
                    }
                    
                } else {
                    if ( m_str[count] != str.m_str[0] ) {
                        match_pos++;
                    } else {
                        matching = true;
                        match_count = 1;
                    }
                }
                
                count++;
            }
            
            if (matching)
                return match_pos;
            else
                return npos;
        }
        euint rfind ( const string_base_old &str, euint pos = npos ) const {
            if ( pos == npos ) {
                pos = m_size - 1;
            }
            
            if ( !m_size || !str.m_size ) {
                return npos;
            }
            
            esint count = pos;
            esint match_count = ( esint ) str.m_size - 1;
            esint match_pos = ( esint ) pos;
            bool matching = false;
            
            while ( count >= 0 ) {
                if ( matching ) {
                    if ( match_count < 0 ) {
                        return ( euint ) match_pos - str.m_size + 1;
                    }
                    
                    if ( m_str[count] != str.m_str[match_count] ) {
                        match_count = str.m_size - 1;
                        matching = false;
                        match_pos = count - 1;
                    }
                    else {
                        match_count--;
                    }
                } else {
                    if ( m_str[count] != str.m_str[str.m_size - 1] ) {
                        match_pos--;
                    } else {
                        matching = true;
                        match_count = str.m_size - 2;
                    }
                }
                
                count--;
            }
            
            if (matching)
                return match_pos;
            else
                return npos;
        }
        euint find ( const C *str, euint pos = 0 ) const {
            string_base_old tmp ( str );
            return find ( tmp, pos );
        }
        euint rfind ( const C *str, euint pos = npos ) const {
            string_base_old tmp ( str );
            return rfind ( tmp, pos );
        }
        euint find_first_of ( const string_base_old &str, euint pos = 0 ) const {
            return find ( str, pos );
        }
        euint find_first_of ( const C *s, euint pos = 0 ) const {
            return find ( s, pos );
        }
        euint find_last_of ( const string_base_old &str, euint pos = npos ) const {
            return rfind ( str, pos );
        }
        euint find_last_of ( const C *s, euint pos = npos ) const {
            return rfind ( s, pos );
        }
        string_base_old substr ( euint pos = 0, euint len = npos ) const {
            if ( pos >= m_size ) {
                return string_base_old();
            }
            
            if ( pos == 0 && len == npos ) {
                return string_base_old ( *this );
            }
            
            if ( len + pos > m_size ) {
                len = m_size - pos;
            }
            
            C *str = ( C * ) NMalloc ( (len + 1) * sizeof(C) );
            memcpy ( str, &m_str[pos], len * sizeof(C) );
            str[len] = 0;
            string_base_old ret;
            Mfree ( ret.m_str );
            ret.m_str = str;
            ret.m_size = len;
            return ret;
        }
        vector< string_base_old > split( C ch ) const {
            vector< string_base_old > ret;
            euint subStrSize = 0;
            euint begin = 0;
            euint count = 0;
            while (m_str[count]) {
                if (m_str[count] == ch) {
                    subStrSize = count - begin;
                    if (subStrSize) {
                        string_base_old tmp(&m_str[begin], subStrSize);
                        ret.push_back(tmp);
                    }
                    begin = count + 1;
                }
                count++;
            }
            if (count) {
                subStrSize = count - begin;
                count--;
                if (subStrSize) {
                    string_base_old tmp(&m_str[begin], subStrSize);
                    ret.push_back(tmp);
                }
            }
            return ret;
        }
        euint size() const {
            return m_size;
        }
        
        inline euint32 get_hash() const {
            int count = 0;
            while (m_str[count]) {
                count++;
            }
            return calc_hashnr ( (const char*)m_str, count * sizeof(C) );
        }
        
        inline euint32 hash_value() const {
            return get_hash();
        }
        
        inline void update_hash_status( ::hash_calc_status* status ) const {
            int count = 0;
            while (m_str[count]) {
                count++;
            }
            ::update_hash_status(status, (const char*)m_str, count * sizeof(C) );
        }
        
        C& operator[] (euint pos) const {
            return m_str[pos];
        }
        void clear() {
            if (m_own_str) {
                Mfree(m_str);
            }
            m_own_str = true;
            m_str = ( C * ) NMalloc ( sizeof(C) );
            m_str[0] = 0;
            m_size = 0;
        }
        void resize(euint newSize) {
            if (m_size < newSize) {
                C *tmp = ( C * ) SMalloc ( (newSize + 1) * sizeof(C) );
                if (m_size) {
                    memcpy(tmp, m_str, m_size * sizeof(C));
                }
                if (m_own_str) {
                    Mfree(m_str);
                }
                m_own_str = true;
                m_str = tmp;
            }
            m_size = newSize;
        }
    };

template <typename C, unsigned NUM_EXTENDED_WORDS, typename STR_LEN_PROC, typename STR_CMP_PROC, typename DEFAULT_STR_PROC>
class string_base
{
private:
    union
    {
        string_data0<C, NUM_EXTENDED_WORDS> data0;
        string_data1<C, NUM_EXTENDED_WORDS> data1;
    } m_data;
    STR_LEN_PROC m_str_len_proc;
    STR_CMP_PROC m_str_cmp_proc;
    DEFAULT_STR_PROC m_default_str_proc;
#if TESTING_STRING_NEW
    string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC> m_old_string;
#endif
public:
    inline void set_own_str(bool flag) {
        if (flag) { m_data.data1.m_flag |= 0x80; }
        else      { m_data.data1.m_flag &= 0x7f; }
    }
    inline bool get_own_str() const {
        return m_data.data1.m_flag & 0x80;
    }
    inline void set_using_data1(bool flag) {
        if (flag) { m_data.data1.m_flag |= 0x40; }
        else      { m_data.data1.m_flag &= 0xbf; }
    }
    inline bool get_using_data1() const {
        return m_data.data1.m_flag & 0x40;
    }
    inline void set_data1_size(euint8 size) {
        m_data.data1.m_flag = (m_data.data1.m_flag & 0xc0) | (size & 0x3f);
    }
    inline euint8 get_data1_size() const {
        return m_data.data1.m_flag & 0x3f;
    }
#if TESTING_STRING_NEW
    void test_string()
    {
        EAssert(m_str_cmp_proc(c_str(), m_old_string.c_str()) == 0, "error");
    }
#endif
public:
    const static euint npos = ( euint )-1;
    typedef C* iterator;
    typedef const C* const_iterator;
    string_base() {
        set_own_str( true );
        set_using_data1( true );
        set_data1_size( 0 );
        m_data.data1.m_str[0] = 0;
#if TESTING_STRING_NEW
        test_string();
#endif
    }
    string_base ( const C *str, bool own_str = true ) {
        if (!str) {
            set_own_str( true );
            set_using_data1( true );
            set_data1_size( 0 );
            m_data.data1.m_str[0] = 0;
#if TESTING_STRING_NEW
            m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str, own_str);
            test_string();
#endif
            return;
        }
        
        euint count = m_str_len_proc(str);
        
        if (own_str) {
            if (count + 1 > DATA1_SIZE) {
                m_data.data0.m_str = ( C * ) NMalloc ( (count + 1) * sizeof(C) );
                memcpy ( m_data.data0.m_str, str, (count + 1) * sizeof(C) );
                m_data.data0.m_size = count;
                set_using_data1( false );
            }
            else {
                memcpy( m_data.data1.m_str, str, (count + 1) * sizeof(C) );
                set_using_data1( true );
                set_data1_size( count );
            }
        }
        else {
            m_data.data0.m_str = ( C * )str;
            m_data.data0.m_size = count;
            set_using_data1( false );
        }
        set_own_str( own_str );
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str, own_str);
        test_string();
#endif
    }
    string_base ( const C *str, euint size ) {
        set_own_str( true );
        if (!str) {
            set_using_data1( true );
            set_data1_size( 0 );
            m_data.data1.m_str[0] = 0;
#if TESTING_STRING_NEW
            m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str, size);
            test_string();
#endif
            return;
        }
        
        if ( size + 1 > DATA1_SIZE ) {
            m_data.data0.m_str = ( C * ) NMalloc ( (size + 1) * sizeof(C) );
            memcpy ( m_data.data0.m_str, str, size * sizeof(C) );
            m_data.data0.m_str[size] = 0;
            m_data.data0.m_size = size;
            set_using_data1( false );
        }
        else {
            memcpy( m_data.data1.m_str, str, size * sizeof(C) );
            m_data.data1.m_str[size] = 0;
            set_using_data1( true );
            set_data1_size( size );
        }
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str, size);
        test_string();
#endif
    }
	string_base ( const vector< C, FGetCharRealSizeProc<C> >& str ) {
        euint size = str.size();
        set_own_str( true );
        if ( size + 1 > DATA1_SIZE ) {
            m_data.data0.m_str = ( C * ) NMalloc ( (size + 1) * sizeof(C) );
            memcpy ( m_data.data0.m_str, str.get(), size * sizeof(C) );
            m_data.data0.m_str[size] = 0;
            m_data.data0.m_size = size;
            set_using_data1( false );
        }
        else {
            memcpy( m_data.data1.m_str, str.get(), size * sizeof(C) );
            m_data.data1.m_str[size] = 0;
            set_using_data1( true );
            set_data1_size( size );
        }
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str);
        test_string();
#endif
    }
    string_base ( const string_base &str ) {
        if (str.get_own_str()) {
            if (str.get_using_data1()) {
                memcpy( m_data.data1.m_str, str.m_data.data1.m_str, (str.get_data1_size() + 1) * sizeof(C) );
                set_using_data1( true );
                set_data1_size( str.get_data1_size() );
            }
            else {
                m_data.data0.m_str = ( C * ) NMalloc ( (str.m_data.data0.m_size + 1) * sizeof(C) );
                memcpy ( m_data.data0.m_str, str.m_data.data0.m_str, (str.m_data.data0.m_size + 1) * sizeof(C) );
                m_data.data0.m_size = str.m_data.data0.m_size;
                set_using_data1( false );
            }
        }
        else {
            m_data.data0.m_str = str.m_data.data0.m_str;
            m_data.data0.m_size = str.m_data.data0.m_size;
        }
        set_own_str( str.get_own_str() );
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str.c_str());
        test_string();
#endif
    }
    string_base ( string_base &&str ) {
        if (str.get_own_str()) {
            if (str.get_using_data1()) {
                memcpy( m_data.data1.m_str, str.m_data.data1.m_str, (str.get_data1_size() + 1) * sizeof(C) );
                set_using_data1( true );
                set_data1_size( str.get_data1_size() );
            }
            else {
                m_data.data0.m_str = ( C * ) NMalloc ( (str.m_data.data0.m_size + 1) * sizeof(C) );
                memcpy ( m_data.data0.m_str, str.m_data.data0.m_str, (str.m_data.data0.m_size + 1) * sizeof(C) );
                m_data.data0.m_size = str.m_data.data0.m_size;
                set_using_data1( false );
            }
        }
        else {
            m_data.data0.m_str = str.m_data.data0.m_str;
            m_data.data0.m_size = str.m_data.data0.m_size;
        }
        set_own_str( str.get_own_str() );
        str.set_own_str( false );
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str.c_str());
        test_string();
#endif
    }
    ~string_base() {
        if (get_own_str() && !get_using_data1()) {
            Mfree ( m_data.data0.m_str );
        }
    }
    iterator begin() {
        if (get_using_data1()) {
            return m_data.data1.m_str;
        }
        else {
            return m_data.data0.m_str;
        }
    }
    iterator end() {
        if (get_using_data1()) {
            return &m_data.data1.m_str[get_data1_size()];
        }
        else {
            return &m_data.data0.m_str[m_data.data0.m_size];
        }
    }
    const_iterator begin() const {
        if (get_using_data1()) {
            return m_data.data1.m_str;
        }
        else {
            return m_data.data0.m_str;
        }
    }
    const_iterator end() const {
        if (get_using_data1()) {
            return &m_data.data1.m_str[get_data1_size()];
        }
        else {
            return &m_data.data0.m_str[m_data.data0.m_size];
        }
    }
    const C *c_str() const {
        if (get_using_data1()) {
            return m_data.data1.m_str;
        }
        else {
            return m_data.data0.m_str;
        }
    }
    bool operator == ( const C* s ) const {
        if (get_using_data1()) {
            return m_str_cmp_proc(m_data.data1.m_str, s) == 0;
        }
        else {
            return m_str_cmp_proc(m_data.data0.m_str, s) == 0;
        }
    }
    bool operator != ( const C* s ) const {
        if (get_using_data1()) {
            return m_str_cmp_proc(m_data.data1.m_str, s) != 0;
        }
        else {
            return m_str_cmp_proc(m_data.data0.m_str, s) != 0;
        }
    }
    bool operator < ( const string_base &str ) const {
        if (get_using_data1() && str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data1.m_str, str.m_data.data1.m_str ) < 0;
        }
        else if (get_using_data1() && !str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data1.m_str, str.m_data.data0.m_str ) < 0;
        }
        else if (!get_using_data1() && str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data0.m_str, str.m_data.data1.m_str ) < 0;
        }
        else {
            return m_str_cmp_proc ( m_data.data0.m_str, str.m_data.data0.m_str ) < 0;
        }
    }
    bool operator == ( const string_base &str ) const {
        if (get_using_data1() && str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data1.m_str, str.m_data.data1.m_str ) == 0;
        }
        else if (get_using_data1() && !str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data1.m_str, str.m_data.data0.m_str ) == 0;
        }
        else if (!get_using_data1() && str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data0.m_str, str.m_data.data1.m_str ) == 0;
        }
        else {
            return m_str_cmp_proc ( m_data.data0.m_str, str.m_data.data0.m_str ) == 0;
        }
    }
    bool operator != ( const string_base &str ) const {
        if (get_using_data1() && str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data1.m_str, str.m_data.data1.m_str ) != 0;
        }
        else if (get_using_data1() && !str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data1.m_str, str.m_data.data0.m_str ) != 0;
        }
        else if (!get_using_data1() && str.get_using_data1()) {
            return m_str_cmp_proc ( m_data.data0.m_str, str.m_data.data1.m_str ) != 0;
        }
        else {
            return m_str_cmp_proc ( m_data.data0.m_str, str.m_data.data0.m_str ) != 0;
        }
    }
    string_base &operator = ( const string_base &str ) {
        if (get_own_str() && !get_using_data1()) {
            Mfree ( m_data.data0.m_str );
        }
        if (str.get_own_str()) {
            if (str.get_using_data1()) {
                memcpy ( &m_data, &str.m_data, sizeof(m_data) );
                set_data1_size( str.get_data1_size() );
            }
            else {
                m_data.data0.m_str = ( C * ) NMalloc ( (str.m_data.data0.m_size + 1) * sizeof(C) );
                memcpy ( m_data.data0.m_str, str.m_data.data0.m_str, (str.m_data.data0.m_size + 1) * sizeof(C) );
                m_data.data0.m_size = str.m_data.data0.m_size;
            }
            set_using_data1( str.get_using_data1() );
        }
        else {
            m_data.data0.m_str = str.m_data.data0.m_str;
            m_data.data0.m_size = str.m_data.data0.m_size;
            set_using_data1( false );
        }
        set_own_str( str.get_own_str() );
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str.c_str());
        test_string();
#endif
        return *this;
    }
    string_base &operator = ( string_base &&str ) {
        if (get_own_str() && !get_using_data1()) {
            Mfree ( m_data.data0.m_str );
        }
        if (str.get_own_str()) {
            if (str.get_using_data1()) {
                memcpy ( &m_data, &str.m_data, sizeof(m_data) );
                set_data1_size( str.get_data1_size() );
            }
            else {
                m_data.data0.m_str = str.m_data.data0.m_str;
                m_data.data0.m_size = str.m_data.data0.m_size;
            }
            set_using_data1( str.get_using_data1() );
        }
        else {
            m_data.data0.m_str = str.m_data.data0.m_str;
            m_data.data0.m_size = str.m_data.data0.m_size;
            set_using_data1( false );
        }
        set_own_str( str.get_own_str() );
        str.set_own_str( false );
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str.c_str());
        test_string();
#endif
        return *this;
    }
    string_base &operator = ( const C *str ) {
        euint count = 0;

        while ( str[count] ) {
            count++;
        }
        if (get_own_str() && !get_using_data1()) {
            Mfree ( m_data.data0.m_str );
        }
        if ( count + 1 > DATA1_SIZE ) {
            m_data.data0.m_str = ( C * ) NMalloc ( (count + 1) * sizeof(C) );
            memcpy ( m_data.data0.m_str, str, (count + 1) * sizeof(C) );
            m_data.data0.m_size = count;
            set_using_data1( false );
        }
        else {
            memcpy ( m_data.data1.m_str, str, (count + 1) * sizeof(C) );
            set_using_data1( true );
            set_data1_size( count );
        }
        set_own_str( true );
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str);
        test_string();
#endif
        return *this;
    }
	string_base &operator = ( const vector< C, FGetCharRealSizeProc<C> >& str ) {
        euint count = str.size();
        if (get_own_str() && !get_using_data1()) {
            Mfree ( m_data.data0.m_str );
        }
        if ( count + 1 > DATA1_SIZE ) {
            m_data.data0.m_str = ( C * ) NMalloc ( (count + 1) * sizeof(C) );
            memcpy ( m_data.data0.m_str, str.get(), count * sizeof(C) );
            m_data.data0.m_str[count] = 0;
            m_data.data0.m_size = count;
            set_using_data1( false );
        }
        else {
            memcpy ( m_data.data1.m_str, str.get(), count * sizeof(C) );
            m_data.data1.m_str[count] = 0;
            set_using_data1( true );
            set_data1_size( count );
        }
        set_own_str( true );
#if TESTING_STRING_NEW
        m_old_string = string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC>(str);
        test_string();
#endif
        return *this;
    }
    string_base operator + ( const string_base &str ) const {
        string_base ret;
        ret.set_own_str( true );
        if (get_using_data1() && str.get_using_data1()) {
            euint new_size = get_data1_size() + str.get_data1_size();
            if ( new_size + 1 > DATA1_SIZE ) {
                ret.set_using_data1( false );
                ret.m_data.data0.m_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
                memcpy ( ret.m_data.data0.m_str, m_data.data1.m_str, get_data1_size() * sizeof(C) );
                memcpy ( &ret.m_data.data0.m_str[get_data1_size()], str.m_data.data1.m_str, (str.get_data1_size() + 1) * sizeof(C) );
                ret.m_data.data0.m_size = new_size;
            }
            else {
                ret.set_using_data1( true );
                memcpy ( ret.m_data.data1.m_str, m_data.data1.m_str, get_data1_size() * sizeof(C) );
                memcpy ( &ret.m_data.data1.m_str[get_data1_size()], str.m_data.data1.m_str, (str.get_data1_size() + 1) * sizeof(C) );
                ret.set_data1_size( new_size );
            }
        }
        else if (get_using_data1() && !str.get_using_data1()) {
            euint new_size = get_data1_size() + str.m_data.data0.m_size;
            ret.set_using_data1( false );
            ret.m_data.data0.m_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
            memcpy ( ret.m_data.data0.m_str, m_data.data1.m_str, get_data1_size() * sizeof(C) );
            memcpy ( &ret.m_data.data0.m_str[get_data1_size()], str.m_data.data0.m_str, (str.m_data.data0.m_size + 1) * sizeof(C) );
            ret.m_data.data0.m_size = new_size;
        }
        else if (!get_using_data1() && str.get_using_data1()) {
            euint new_size = m_data.data0.m_size + str.get_data1_size();
            ret.set_using_data1( false );
            ret.m_data.data0.m_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
            memcpy ( ret.m_data.data0.m_str, m_data.data0.m_str, m_data.data0.m_size * sizeof(C) );
            memcpy ( &ret.m_data.data0.m_str[m_data.data0.m_size], str.m_data.data1.m_str, (str.get_data1_size() + 1) * sizeof(C) );
            ret.m_data.data0.m_size = new_size;
        }
        else {
            euint new_size = m_data.data0.m_size + str.m_data.data0.m_size;
            ret.set_using_data1( false );
            ret.m_data.data0.m_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
            memcpy ( ret.m_data.data0.m_str, m_data.data0.m_str, m_data.data0.m_size * sizeof(C) );
            memcpy ( &ret.m_data.data0.m_str[m_data.data0.m_size], str.m_data.data0.m_str, (str.m_data.data0.m_size + 1) * sizeof(C) );
            ret.m_data.data0.m_size = new_size;
        }
#if TESTING_STRING_NEW
        string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC> tmp =
        m_old_string + str.c_str();
        EAssert(m_str_cmp_proc(tmp.c_str(), ret.c_str()) == 0, "%s != %s, this: %s, %s", tmp.c_str(), ret.c_str(), m_old_string.c_str(), c_str());
#endif
        return ret;
    }
    string_base operator + ( const C *str ) const {
        euint count = 0;

        while ( str[count] ) {
            count++;
        }

        string_base ret;
        ret.set_own_str( true );
        if (get_using_data1()) {
            euint new_size = get_data1_size() + count;
            if ( new_size + 1 > DATA1_SIZE ) {
                ret.set_using_data1( false );
                ret.m_data.data0.m_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
                memcpy ( ret.m_data.data0.m_str, m_data.data1.m_str, get_data1_size() * sizeof(C) );
                memcpy ( &ret.m_data.data0.m_str[get_data1_size()], str, (count + 1) * sizeof(C) );
                ret.m_data.data0.m_size = new_size;
            }
            else {
                ret.set_using_data1( true );
                memcpy ( ret.m_data.data1.m_str, m_data.data1.m_str, get_data1_size() * sizeof(C) );
                memcpy ( &ret.m_data.data1.m_str[get_data1_size()], str, (count + 1) * sizeof(C) );
                ret.set_data1_size( new_size );
            }
        }
        else {
            euint new_size = m_data.data0.m_size + count;
            ret.set_using_data1( false );
            ret.m_data.data0.m_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
            memcpy ( ret.m_data.data0.m_str, m_data.data0.m_str, m_data.data0.m_size * sizeof(C) );
            memcpy ( &ret.m_data.data0.m_str[m_data.data0.m_size], str, (count + 1) * sizeof(C) );
            ret.m_data.data0.m_size = new_size;
        }
#if TESTING_STRING_NEW
        string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC> tmp =
        m_old_string + str;
        EAssert(m_str_cmp_proc(tmp.c_str(), ret.c_str()) == 0, "error");
#endif
        return ret;
    }
    string_base &operator += ( const string_base &str ) {
        if (get_using_data1() && str.get_using_data1()) {
            euint new_size = get_data1_size() + str.get_data1_size();
            if ( new_size + 1 > DATA1_SIZE ) {
                set_using_data1( false );
                C* new_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
                memcpy ( new_str, m_data.data1.m_str, get_data1_size() * sizeof(C) );
                memcpy ( &new_str[get_data1_size()], str.m_data.data1.m_str, (str.get_data1_size() + 1) * sizeof(C) );
                m_data.data0.m_str = new_str;
                m_data.data0.m_size = new_size;
            }
            else {
                set_using_data1( true );
                memcpy ( &m_data.data1.m_str[get_data1_size()], str.m_data.data1.m_str, (str.get_data1_size() + 1) * sizeof(C) );
                set_data1_size( new_size );
            }
        }
        else if (get_using_data1() && !str.get_using_data1()) {
            euint new_size = get_data1_size() + str.m_data.data0.m_size;
            set_using_data1( false );
            C* new_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
            memcpy ( new_str, m_data.data1.m_str, get_data1_size() * sizeof(C) );
            memcpy ( &new_str[get_data1_size()], str.m_data.data0.m_str, (str.m_data.data0.m_size + 1) * sizeof(C) );
            m_data.data0.m_str = new_str;
            m_data.data0.m_size = new_size;
        }
        else if (!get_using_data1() && str.get_using_data1()) {
            euint new_size = m_data.data0.m_size + str.get_data1_size();
            set_using_data1( false );
            C* new_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
            memcpy ( new_str, m_data.data0.m_str, m_data.data0.m_size * sizeof(C) );
            memcpy ( &new_str[m_data.data0.m_size], str.m_data.data1.m_str, (str.get_data1_size() + 1) * sizeof(C) );
            if (get_own_str()) {
                Mfree(m_data.data0.m_str);
            }
            m_data.data0.m_str = new_str;
            m_data.data0.m_size = new_size;
        }
        else {
            euint new_size = m_data.data0.m_size + str.m_data.data0.m_size;
            set_using_data1( false );
            C* new_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
            memcpy ( new_str, m_data.data0.m_str, m_data.data0.m_size * sizeof(C) );
            memcpy ( &new_str[m_data.data0.m_size], str.m_data.data0.m_str, (str.m_data.data0.m_size + 1) * sizeof(C) );
            if (get_own_str()) {
                Mfree(m_data.data0.m_str);
            }
            m_data.data0.m_str = new_str;
            m_data.data0.m_size = new_size;
        }
#if TESTING_STRING_NEW
        m_old_string += str.c_str();
        test_string();
#endif
        return *this;
    }
    string_base &operator += ( const C *str ) {
        euint count = 0;

        while ( str[count] ) {
            count++;
        }

        if (get_using_data1()) {
            euint new_size = get_data1_size() + count;
            if ( new_size + 1 > DATA1_SIZE ) {
                set_using_data1( false );
                C* new_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
                memcpy ( new_str, m_data.data1.m_str, get_data1_size() * sizeof(C) );
                memcpy ( &new_str[get_data1_size()], str, (count + 1) * sizeof(C) );
                m_data.data0.m_str = new_str;
                m_data.data0.m_size = new_size;
            }
            else {
                set_using_data1( true );
                memcpy ( &m_data.data1.m_str[get_data1_size()], str, (count + 1) * sizeof(C) );
                set_data1_size( new_size );
            }
        }
        else {
            euint new_size = m_data.data0.m_size + count;
            set_using_data1( false );
            C* new_str = ( C * ) NMalloc ( (new_size + 1) * sizeof(C) );
            memcpy ( new_str, m_data.data0.m_str, m_data.data0.m_size * sizeof(C) );
            memcpy ( &new_str[m_data.data0.m_size], str, (count + 1) * sizeof(C) );
            if (get_own_str()) {
                Mfree(m_data.data0.m_str);
            }
            m_data.data0.m_str = new_str;
            m_data.data0.m_size = new_size;
        }
#if TESTING_STRING_NEW
        m_old_string += str;
        test_string();
#endif
        return *this;
    }
    string_base &operator += ( C c ) {
        C buf[2];
        buf[0] = c;
        buf[1] = 0;
        return (this->operator+=)(buf);
    }
    euint find ( const string_base &str, euint pos = 0 ) const {
        const C* _str = get_using_data1() ? m_data.data1.m_str : m_data.data0.m_str;
        euint _size = get_using_data1() ? get_data1_size() : m_data.data0.m_size;
        const C* str_str = str.get_using_data1() ? str.m_data.data1.m_str : str.m_data.data0.m_str;
        euint str_size = str.get_using_data1() ? str.get_data1_size() : m_data.data0.m_size;
        if ( _size <= pos || !_size || !str_size ) {
            return npos;
        }

        euint count = pos;
        euint match_count = 0;
        euint match_pos = pos;
        bool matching = false;

        while ( _str[count] ) {
            if ( matching ) {
                if ( match_count == str_size ) {
                    return match_pos;
                }

                if ( _str[count] != str_str[match_count] ) {
                    match_count = 0;
                    matching = false;
                    match_pos = pos + count + 1;
                }
                else {
                    
                    match_count++;
                }
                
            } else {
                if ( _str[count] != str_str[0] ) {
                    match_pos++;
                } else {
                    matching = true;
                    match_count = 1;
                }
            }

            count++;
        }

		if (matching)
			return match_pos;
		else
            return npos;
    }
    euint rfind ( const string_base &str, euint pos = npos ) const {
        const C* _str = get_using_data1() ? m_data.data1.m_str : m_data.data0.m_str;
        euint _size = get_using_data1() ? get_data1_size() : m_data.data0.m_size;
        const C* str_str = str.get_using_data1() ? str.m_data.data1.m_str : str.m_data.data0.m_str;
        euint str_size = str.get_using_data1() ? str.get_data1_size() : m_data.data0.m_size;
        if ( pos == npos ) {
            pos = _size - 1;
        }

        if ( !_size || !str_size ) {
            return npos;
        }

        esint count = pos;
        esint match_count = ( esint ) str_size - 1;
        esint match_pos = ( esint ) pos;
        bool matching = false;

        while ( count >= 0 ) {
            if ( matching ) {
                if ( match_count < 0 ) {
                    return ( euint ) match_pos - str_size + 1;
                }

                if ( _str[count] != str_str[match_count] ) {
                    match_count = str_size - 1;
                    matching = false;
                    match_pos = count - 1;
                }
                else {
                    match_count--;
                }
            } else {
                if ( _str[count] != str_str[str_size - 1] ) {
                    match_pos--;
                } else {
                    matching = true;
                    match_count = str_size - 2;
                }
            }

            count--;
        }

		if (matching)
			return match_pos;
		else
            return npos;
    }
    euint find ( const C *str, euint pos = 0 ) const {
        string_base tmp ( str );
        return find ( tmp, pos );
    }
    euint rfind ( const C *str, euint pos = npos ) const {
        string_base tmp ( str );
        return rfind ( tmp, pos );
    }
    euint find_first_of ( const string_base &str, euint pos = 0 ) const {
        return find ( str, pos );
    }
    euint find_first_of ( const C *s, euint pos = 0 ) const {
        return find ( s, pos );
    }
    euint find_last_of ( const string_base &str, euint pos = npos ) const {
        return rfind ( str, pos );
    }
    euint find_last_of ( const C *s, euint pos = npos ) const {
        return rfind ( s, pos );
    }
    string_base substr ( euint pos = 0, euint len = npos ) const {
        const C* _str = get_using_data1() ? m_data.data1.m_str : m_data.data0.m_str;
        euint _size = get_using_data1() ? get_data1_size() : m_data.data0.m_size;
        if ( pos >= _size ) {
            return string_base();
        }

        if ( pos == 0 && len == npos ) {
            return string_base ( *this );
        }

        if ( len + pos > _size ) {
            len = _size - pos;
        }
        
        string_base ret(&_str[pos], len);
#if TESTING_STRING_NEW
        string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC> tmp = m_old_string.substr(pos, len);
        EAssert(m_str_cmp_proc(tmp.c_str(), ret.c_str()) == 0, "error");
#endif
        return ret;
    }
    vector< string_base > split( C ch ) const {
        const C* _str = get_using_data1() ? m_data.data1.m_str : m_data.data0.m_str;
        vector< string_base > ret;
        euint subStrSize = 0;
        euint begin = 0;
        euint count = 0;
        while (_str[count]) {
            if (_str[count] == ch) {
                subStrSize = count - begin;
                if (subStrSize) {
                    string_base tmp(&_str[begin], subStrSize);
                    ret.push_back(tmp);
                }
                begin = count + 1;
            }
            count++;
        }
        if (count) {
            subStrSize = count - begin;
            count--;
            if (subStrSize) {
                string_base tmp(&_str[begin], subStrSize);
                ret.push_back(tmp);
            }
        }
        return ret;
    }
    euint size() const {
        return get_using_data1() ? get_data1_size() : m_data.data0.m_size;
    }

    inline euint32 get_hash() const {
        const C* _str = get_using_data1() ? m_data.data1.m_str : m_data.data0.m_str;
        euint _size = get_using_data1() ? get_data1_size() : m_data.data0.m_size;
        return calc_hashnr ( (const char*)_str, _size * sizeof(C) );
    }
    
    inline euint32 hash_value() const {
        return get_hash();
    }
    
    inline void update_hash_status( ::hash_calc_status* status ) const {
        const C* _str = get_using_data1() ? m_data.data1.m_str : m_data.data0.m_str;
        euint _size = get_using_data1() ? get_data1_size() : m_data.data0.m_size;
        ::update_hash_status(status, (const char*)_str, _size * sizeof(C) );
    }
    
    C& operator[] (euint pos) const {
        C* _str = get_using_data1() ? (C*)m_data.data1.m_str : (C*)m_data.data0.m_str;
        return _str[pos];
    }
	void clear() {
        if (get_own_str() && !get_using_data1()) {
            Mfree(m_data.data0.m_str);
        }
        set_own_str( true );
        set_using_data1( true );
        set_data1_size( 0 );
        m_data.data1.m_str[0] = 0;
#if TESTING_STRING_NEW
        m_old_string.clear();
#endif
	}
    
	void resize(euint newSize) {
        const C* _str = get_using_data1() ? m_data.data1.m_str : m_data.data0.m_str;
        euint _size = get_using_data1() ? get_data1_size() : m_data.data0.m_size;
        C *tmp = ( C * ) SMalloc ( (newSize + 1) * sizeof(C) );
        if (_size) {
            memcpy(tmp, _str, _size * sizeof(C));
        }
        if (get_own_str() && !get_using_data1()) {
            Mfree(m_data.data0.m_str);
        }
        set_using_data1( false );
        m_data.data0.m_str = tmp;
        m_data.data0.m_size = newSize;
#if TESTING_STRING_NEW
        m_old_string.resize(newSize);
#endif
	}
};
    

#if USING_STRING_OLD
    
    template <typename C, unsigned NUM_EXTENDED_WORDS, typename STR_LEN_PROC, typename STR_CMP_PROC, typename DEFAULT_STR_PROC>
    inline euint32 _hash ( const string_base_old<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC> &key )
    {
        return key.get_hash();
    }
    
#else

template <typename C, unsigned NUM_EXTENDED_WORDS, typename STR_LEN_PROC, typename STR_CMP_PROC, typename DEFAULT_STR_PROC>
inline euint32 _hash ( const string_base<C, NUM_EXTENDED_WORDS, STR_LEN_PROC, STR_CMP_PROC, DEFAULT_STR_PROC> &key )
{
    return key.get_hash();
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
