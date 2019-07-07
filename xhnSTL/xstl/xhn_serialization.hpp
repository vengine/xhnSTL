/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_SERIALIZATION_HPP
#define XHN_SERIALIZATION_HPP

#ifdef __cplusplus

#include "common.h"
#include "etypes.h"
#include "xhn_vector.hpp"

namespace xhn
{

template <typename T, bool IS_LITTLE_ENDIAN>
void write_stream(T value, vector<euint8>& result)
{
    euint8* value_ptr = (euint8*)&value;
    for (euint i = 0; i < sizeof(T); i++) {
        if (IS_LITTLE_ENDIAN) {
            result.push_back(value_ptr[i]);
        } else {
            result.push_back(value_ptr[sizeof(T) - i - 1]);
        }
    }
}

template <typename T, bool IS_LITTLE_ENDIAN>
euint read_stream(const vector<euint8>& stream, euint offset, T& result)
{
    euint8* result_ptr = (euint8*)&result;
    for (euint i = 0; i < sizeof(T); i++) {
        if (IS_LITTLE_ENDIAN) {
            result_ptr[i] = stream[i + offset];
        } else {
            result_ptr[sizeof(T) - i - 1] = stream[i + offset];
        }
    }
    return sizeof(T);
}

template <typename T, bool IS_LITTLE_ENDIAN>
class bit_stream_encoder
{
public:
    euint8 m_temp_byte = 0;
    euint8 m_bit_count = 0;
public:
    void write_bit(bool bit, vector<euint8>& result)
    {
        if (bit) {
            m_temp_byte |= (1 << m_bit_count);
        }
        m_bit_count++;
        if (8 == m_bit_count) {
            result.push_back(m_temp_byte);
            
            m_temp_byte = 0;
            m_bit_count = 0;
        }
    }

    void write_value_in_bits(T value, euint num_bits, vector<euint8>& result)
    {
        euint num_bytes = num_bits / 8;
        num_bytes += num_bits % 8 ? 1 : 0;
        euint8* value_ptr = (euint8*)&value;
        euint bit_count = 0;
        for (euint i = 0; i < num_bytes; i++) {
            euint8 byte = 0;
            if (IS_LITTLE_ENDIAN) {
                byte = value_ptr[i];
            } else {
                byte = value_ptr[sizeof(T) - i - 1];
            }
            for (euint j = 0; j < 8; j++) {
                write_bit(byte & (1 << j), result);
                bit_count++;
                if (bit_count == num_bits)
                    return;
            }
        }
    }

    void flush(vector<euint8>& result) {
        if (m_bit_count) {
            result.push_back(m_temp_byte);
            m_temp_byte = 0;
            m_bit_count = 0;
        }
    }
};

template <typename T, bool IS_LITTLE_ENDIAN>
class bit_stream_decoder
{
public:
    euint8 m_temp_byte = 0;
    euint8 m_remainder_bits = 0;
    euint m_byte_offset = 0;
public:
    bool read_bit(const vector<euint8>& stream) {
        if (!m_remainder_bits) {
            if (stream.size() <= m_byte_offset)
                return false;
            m_temp_byte = stream[m_byte_offset++];
            m_remainder_bits = 8;
        }
        return m_temp_byte & (1 << (8 - (m_remainder_bits--)));
    }
    void read_value_in_bits(const vector<euint8>& stream, euint num_bits, T& result)
    {
        euint num_bytes = num_bits / 8;
        num_bytes += num_bits % 8 ? 1 : 0;
        result = 0;
        euint8* result_ptr = (euint8*)&result;
        euint bit_count = 0;
        for (euint i = 0; i < num_bytes; i++) {
            euint8 byte = 0;
            for (euint j = 0; j < 8; j++) {
                if (read_bit(stream)) { byte |= (1 << j); }
                bit_count++;
                if (bit_count == num_bits) {
                    if (IS_LITTLE_ENDIAN) {
                        result_ptr[i] = byte;
                    } else {
                        result_ptr[sizeof(T) - i - 1] = byte;
                    }
                    return;
                }
            }
            if (IS_LITTLE_ENDIAN) {
                result_ptr[i] = byte;
            } else {
                result_ptr[sizeof(T) - i - 1] = byte;
            }
        }
    }
    void seek(const vector<euint8>& stream, euint bit_offset) {
        m_byte_offset = bit_offset / 8;
        m_remainder_bits = 8 - bit_offset % 8;
        if (m_byte_offset < stream.size())
            m_temp_byte = stream[m_byte_offset++];
        else
            m_temp_byte = 0;
    }
};

static euint32 encode_uint(euint32 number, euint8* result) 
{
    euint32 index = 0;
    if ((number > 0x7f) != 0) {
        result[index++] = (euint8)(number | 0x80);
        number >>= 7;
        if (number > 0x7f) {
            result[index++] = (euint8)(number | 0x80);
            number >>= 7;
            if (number > 0x7f) {
                result[index++] = (euint8)(number | 0x80);
                number >>= 7;
                if (number > 0x7f) {
                    result[index++] = (euint8)(number | 0x80);
                    number >>= 7;
                }
            }
        }
    } 
    result[index++] = (euint8) number;
    return index;
}

static euint32 decode_uint(euint8* stream, euint32& result) 
{
    euint32 index = 1;
    euint32 byte = stream[0] & 0xff;
    result = byte & 0x7f;
    if (byte & 0x80) {
        byte = stream[index++] & 0xff;
        result |= (byte & 0x7f) << 7;
        if (byte & 0x80) {
            byte = stream[index++] & 0xff;
            result |= (byte & 0x7f) << 14;
            if (byte & 0x80) {
                byte = stream[index++] & 0xff;
                result |= (byte & 0x7f) << 21;
                if (byte & 0x80) {
                    byte = stream[index++] & 0xff;
                    result |= (byte & 0x7f) << 28;
                    if (byte & 0x80) {
                        return 0;
                    }
                }
            }
        }
    }
    return index;
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
