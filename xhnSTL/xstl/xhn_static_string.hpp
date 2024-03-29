/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_STATIC_STRING_HPP
#define XHN_STATIC_STRING_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "eassert.h"
#include "xhn_hash_set.hpp"
#include "xhn_string.hpp"
#include "xhn_utility.hpp"
#include "xhn_pair.hpp"
#include "xhn_dictionary.hpp"
namespace xhn
{
    
struct string_info
{
    euint32 m_hash_value;
    euint32 m_size;
    inline bool operator == (const string_info info) const {
        return m_hash_value == info.m_hash_value &&
        m_size == info.m_size;
    }
    inline bool operator != (const string_info info) const {
        return m_hash_value != info.m_hash_value ||
        m_size != info.m_size;
    }
};

struct FCityHashProc
{
    euint32 operator() (const string& v) const {
        euint32 size = (euint32)v.size();
        euint32 hash_value = calc_cityhash32 ( v.c_str(), size );
        return hash_value;
    }
};
    
class XHN_EXPORT static_string
{
private:
    static dictionary< string, string_info, 2, FCityHashProc >* s_static_string_set;
private:
    dictionary< string, string_info, 2, FCityHashProc >::node_pointer m_entry;
public:
    static const static_string empty_string;
public:
	static void dest();
    static_string ( const char *str ) {
        if (!s_static_string_set) {
            s_static_string_set = VNEW dictionary< string, string_info, 2, FCityHashProc >;
        }

        euint32 size = (euint32)strlen(str);
        euint32 hash_value = calc_cityhash32 ( str, size );
        string_info info = { hash_value, size };
        string value = str;
        auto node = s_static_string_set->find_hash_node( value );
        if (node) {
            m_entry = node;
        }
        else {
            m_entry = s_static_string_set->_insert_node( value, info );
        }
        EDebugAssert(m_entry, "entry must be nonnull");
    }
    static_string ( const static_string& str ) {
        m_entry = str.m_entry;
        EDebugAssert(m_entry, "entry must be nonnull");
    }
    static_string ( static_string&& str ) {
        m_entry = str.m_entry;
        EDebugAssert(m_entry, "entry must be nonnull");
    }
    static_string () {
        if (!s_static_string_set) {
            s_static_string_set = new dictionary< string, string_info, 2, FCityHashProc >;
        }
        string value ( "" );
        string_info info = { 0, 0, };
        
        auto node = s_static_string_set->find_hash_node( value );
        if (node) {
            m_entry = node;
        }
        else {
            m_entry = s_static_string_set->_insert_node( value, info );
        }
        EDebugAssert(m_entry, "entry must be nonnull");
    }
    static_string (int) {
        if (!s_static_string_set) {
            s_static_string_set = new dictionary< string, string_info, 2, FCityHashProc >;
        }
        string value ( "" );
        string_info info = { 0, 0, };
        
        auto node = s_static_string_set->find_hash_node( value );
        if (node) {
            m_entry = node;
        }
        else {
            m_entry = s_static_string_set->_insert_node( value, info );
        }
        EDebugAssert(m_entry, "entry must be nonnull");
    }
    const char *c_str() const {
        EDebugAssert(m_entry, "entry must be nonnull");
        return m_entry->first.c_str();
    }
    bool operator < ( const static_string &str ) const {
        EDebugAssert(m_entry, "entry must be nonnull");
        return m_entry < str.m_entry;
    }
    bool operator > ( const static_string &str ) const {
        EDebugAssert(m_entry, "entry must be nonnull");
        return m_entry > str.m_entry;
    }
    bool operator == ( const static_string &str ) const {
        EDebugAssert(m_entry, "entry must be nonnull");
        return m_entry == str.m_entry;
    }
    bool operator != ( const static_string &str ) const {
        EDebugAssert(m_entry, "entry must be nonnull");
        return m_entry != str.m_entry;
    }
    euint size() const {
        EDebugAssert(m_entry, "entry must be nonnull");
        return m_entry->second.m_size;
    }
    euint32 hash_value() const
    {
        EDebugAssert(m_entry, "entry must be nonnull");
        return m_entry->second.m_hash_value;
    }
    const static_string& operator = (const static_string& str) {
        m_entry = str.m_entry;
        EDebugAssert(m_entry, "entry must be nonnull");
        return *this;
    }
    const static_string& operator = (static_string&& str) {
        m_entry = str.m_entry;
        EDebugAssert(m_entry, "entry must be nonnull");
        return *this;
    }
    void update_hash_status( ::hash_calc_status* status ) const {
        ::update_hash_status(status, (const char*)m_entry, sizeof(m_entry) );
    }
};
    
inline euint32 _hash ( const static_string& key )
{
    return key.hash_value();
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
