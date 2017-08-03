/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "xhn_exception.hpp"
#include "xhn_string.hpp"

namespace xhn {

class ExceptionBody : public MemObject
{
public:
	const char* m_file;
	euint32 m_line;
	xhn::string m_msg;
	xhn::string m_what;
public:
	ExceptionBody(const char* file, euint32 line, const char* msg)
		: m_file(file)
		, m_line(line)
		, m_msg(msg)
	{
	}
};
Exception::Exception(const char* file, euint32 line, const char* msg) {
    m_body = VNEW ExceptionBody(file, line, msg);
	elog("error: %s", what());
}
Exception::~Exception() {
    delete m_body;
}
const char* Exception::what() {
	const RTTI* rtti = GetRTTI();
    char mbuf[256];
    snprintf(mbuf, 255, "%d", m_body->m_line);
	m_body->m_what = "Error;";
    m_body->m_what += m_body->m_file;
    m_body->m_what += ";";
    m_body->m_what += mbuf;
    m_body->m_what += ";";
	m_body->m_what += rtti->GetTypeName();
    m_body->m_what += ";";
	m_body->m_what += m_body->m_msg;

	return m_body->m_what.c_str();
}

ImplementRootRTTI(Exception);
ImplementRTTI(ThreadException, Exception);
ImplementRTTI(FunctionException, Exception);
ImplementRTTI(ObjectException, Exception);
ImplementRTTI(MemoryException, Exception);

ImplementRTTI(FunctionArgumentException, FunctionException);
ImplementRTTI(FunctionExecutionException, FunctionException);
ImplementRTTI(InvalidOperationException, FunctionExecutionException);
ImplementRTTI(UnexpectedValueException, FunctionExecutionException);
ImplementRTTI(UndesiredBranch, FunctionExecutionException);
ImplementRTTI(FunctionResultException, FunctionException);
ImplementRTTI(SubscriptOutOfBounds, FunctionExecutionException);
ImplementRTTI(InvalidEnumerationException, FunctionExecutionException);

ImplementRTTI(ObjectNameAlreadyExistedException, ObjectException);
ImplementRTTI(ObjectAlreadyExistedException, ObjectException);
ImplementRTTI(ObjectUninitializedException, ObjectException);
ImplementRTTI(ObjectSubscriptOutOfRange, ObjectException);
ImplementRTTI(ObjectNotExistedException, ObjectException);

ImplementRTTI(InvalidMemoryAllocException, MemoryException);
ImplementRTTI(InvalidMemoryFreeException, MemoryException);
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