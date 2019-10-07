/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "xhn_string.hpp"

#if defined(__APPLE__)
#include "ObjCString.h"
#elif defined(LINUX)
#include "LinuxString.hpp"
#elif defined(_WIN32) || defined(_WIN64)
#include "WinString.hpp"
#endif

wchar_t get_char(euint i)
{
    switch (i)
    {
        case 0:
            return L'0';
        case 1:
            return L'1';
        case 2:
            return L'2';
        case 3:
            return L'3';
        case 4:
            return L'4';
        case 5:
            return L'5';
        case 6:
            return L'6';
        case 7:
            return L'7';
        case 8:
            return L'8';
        default:
            return L'9';
    }
}
void xhn::itowa(wchar_t* wbuf, euint i)
{
    euint numDigit = 0;
    euint value = i;
    while (value / 10) {
        numDigit++;
        value /= 10;
    }
    numDigit++;
    value = i;
    for (euint j = 0; j < numDigit; j++) {
        wbuf[numDigit - j - 1] = get_char(value % 10);
        value /= 10;
    }
    wbuf[numDigit] = 0;
}

xhn::Utf8::Utf8(const wchar_t* wsz)
{
	vector< char, FGetCharRealSizeProc<char> > buf;
	utf8fromwcs(wsz, buf);
	m_utf8 = string(buf);
}

xhn::Unicode::Unicode(const char* sz)
{
    vector< wchar_t, FGetCharRealSizeProc<wchar_t> > buf;
	utf8towcs(sz, buf);
	m_unicode = wstring(buf);
}

xhn::Unicode::Unicode(const string sz)
{
    vector< wchar_t, FGetCharRealSizeProc<wchar_t> > buf;
	utf8towcs(sz.c_str(), buf);
	m_unicode = wstring(buf);
}

xhn::Unicode::Unicode(euint number)
{
    char mbuf[256];
    euint strlen = 0;
#if   BIT_WIDTH == 32
    if (number <= 999)
        strlen = snprintf(mbuf, 256, "%03d", number);
    else if (number > 999 && number <= 9999)
        strlen = snprintf(mbuf, 256, "%04d", number);
    else if (number > 9999 && number <= 99999)
        strlen = snprintf(mbuf, 256, "%05d", number);
    else if (number > 99999 && number <= 999999)
        strlen = snprintf(mbuf, 256, "%06d", number);
    else
        strlen = snprintf(mbuf, 256, "%d", number);
#elif BIT_WIDTH == 64
    if (number <= 999)
        strlen = snprintf(mbuf, 256, "%03lld", number);
    else if (number > 999 && number <= 9999)
        strlen = snprintf(mbuf, 256, "%04lld", number);
    else if (number > 9999 && number <= 99999)
        strlen = snprintf(mbuf, 256, "%05lld", number);
    else if (number > 99999 && number <= 999999)
        strlen = snprintf(mbuf, 256, "%06lld", number);
    else
        strlen = snprintf(mbuf, 256, "%lld", number);
#else
#    error
#endif
    m_unicode.resize(strlen);
    for (euint i = 0; i < strlen; i++) {
        m_unicode[i] = mbuf[i];
    }
}

bool xhn::Utf8::utf8fromwcs(const wchar_t* wcs, vector< char, FGetCharRealSizeProc<char> >& outbuf)
{
	const wchar_t *pc = wcs;
	euint num_errors = 0;
	///int i = 0;
	for(unsigned int c = *pc; c != 0 ; c = *(++pc))
	{
		if (c < (1 << 7))
		{
			outbuf.push_back(char(c));
		}
		else if (c < (1 << 11))
		{
			outbuf.push_back(char((c >> 6) | 0xc0));
			outbuf.push_back(char((c & 0x3f) | 0x80));
		}
		else if (c < (1 << 16))
		{
			outbuf.push_back(char((c >> 12) | 0xe0));
			outbuf.push_back(char(((c >> 6) & 0x3f) | 0x80));
			outbuf.push_back(char((c & 0x3f) | 0x80));
		}
		else if (c < (1 << 21))
		{
			outbuf.push_back(char((c >> 18) | 0xf0));
			outbuf.push_back(char(((c >> 12) & 0x3f) | 0x80));
			outbuf.push_back(char(((c >> 6) & 0x3f) | 0x80));
			outbuf.push_back(char((c & 0x3f) | 0x80));
		}
		else
			++num_errors;
	}
	return num_errors == 0;
}

bool xhn::Unicode::utf8towcs(const char *utf8, vector< wchar_t, FGetCharRealSizeProc<wchar_t> >& outbuf)
{
	if(!utf8) return true;
	euint count = 0;
	const char* pc = (const char*)utf8;
	const char* last = NULL;
	euint b;
	euint num_errors = 0;
	int i = 0;
	while (utf8[count]) {
		count++;
	}
	if (!count) return true;
	last = &utf8[count];
	while (*pc)
	{
		b = *pc++;

        b &= 0xff;
		if( !b ) break; // 0 - is eos in all utf encodings

		if ((b & 0x80) == 0)
		{
			// 1-byte sequence: 000000000xxxxxxx = 0xxxxxxx
            outbuf.push_back(wchar_t(b));
		}
		else if ((b & 0xe0) == 0xc0)
		{
			// 2-byte sequence: 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
			if(pc == last) {
                outbuf.push_back('?'); ++num_errors; break;
            }
			b = (b & 0x1f) << 6;
			b |= (*pc++ & 0x3f);
            outbuf.push_back(wchar_t(b));
		}
		else if ((b & 0xf0) == 0xe0)
		{
			// 3-byte sequence: zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 1) {
                outbuf.push_back('?'); ++num_errors; break;
            }
			b = (b & 0x0f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			if(b == 0xFEFF &&
				i == 0) // bom at start
				continue; // skip it
            outbuf.push_back(wchar_t(b));
		}
		else if ((b & 0xf8) == 0xf0)
		{
#if defined(USAGE_UCS2)
            /// 大于16bit的unicode是不被允许的
            outbuf.clear();
            char* tmp = string_convert_to_utf8(utf8);
            utf8towcs(tmp, outbuf);
            free(tmp);
            return true;
#elif defined(USAGE_UCS4)
            // 4-byte sequence: uuuzzzzzzyyyyyyxxxxxx = 11110uuu 10zzzzzz 10yyyyyy 10xxxxxx

			if(pc >= last - 2) {
                outbuf.push_back('?'); break;
            }
			b = (b & 0x07) << 18;
			b |= (*pc++ & 0x3f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			// b shall contain now full 21-bit unicode code point.
			assert((b & 0x1fffff) == b);
			if((b & 0x1fffff) != b)
			{
				outbuf.push_back('?');
				++num_errors;
				continue;
			}
            euint wchar_tSize = sizeof(wchar_t);
			if( wchar_tSize == 2 ) // Seems like Windows, wchar_t is utf16 code units sequence there.
			{
				outbuf.push_back(wchar_t(0xd7c0 + (b >> 10)));
				outbuf.push_back(wchar_t(0xdc00 | (b & 0x3ff)));
			}
			else if( wchar_tSize == 4 ) // wchar_t is full ucs-4
			{
				outbuf.push_back(wchar_t(b));
			}
			else
			{
				assert(0); // what? wchar_t is single byte here?
			}
#else
#    error
#endif
        }
		else
		{
            outbuf.clear();
            char* tmp = string_convert_to_utf8(utf8);
            utf8towcs(tmp, outbuf);
            free(tmp);
            return true;
		}
	}
	return num_errors == 0;
}

xhn::string xhn::to_url(const xhn::string& utf8_path)
{
    string ret;
    const char* buf = convert_utf8_path_to_url(utf8_path.c_str());
    if (buf) {
        ret = buf;
        free((char*)buf);
    }
    return ret;
}

xhn::string xhn::to_utf8_path(const string& url)
{
    string ret;
    const char* buf = convert_url_to_utf8_path(url.c_str());
    if (buf) {
        ret = buf;
        free((char*)buf);
    }
    return ret;
}

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
