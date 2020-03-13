/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_EXCEPTION_HPP
#define XHN_EXCEPTION_HPP

#ifdef __cplusplus

#include <exception>
#include "rtti.hpp"
#include "elog.h"
/**
#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_OSX
#include "osx_dialog.h"
#endif
#endif
**/
namespace xhn {
class ExceptionBody;
class XHN_EXPORT Exception : public std::exception
{
    DeclareRootRTTI;
public:
	ExceptionBody* m_body;
public:
    Exception(const char* file, euint32 line, const char* msg);
	~Exception();
    const char* what();
};
/// 对于不同的异常有不同的处理方案
/// 资源异常的话给予默认资源，并报错
class XHN_EXPORT ResourceException : public Exception
{
    DeclareRTTI;
public:
	ResourceException(const char* file, euint32 line, const char* msg)
    : Exception(file, line, msg)
	{}
};
/// 线程异常包括取消线程等一般不catch
class XHN_EXPORT ThreadException : public Exception
{
    DeclareRTTI;
public:
	ThreadException(const char* file, euint32 line, const char* msg)
    : Exception(file, line, msg)
	{}
};
/// 函数异常比较严重，直接退出程序
class XHN_EXPORT FunctionException : public Exception
{
	DeclareRTTI;
public:
	FunctionException(const char* file, euint32 line, const char* msg)
		: Exception(file, line, msg)
	{}
};
/// OpenGL异常只是显示错误，静默式报告错误即可
class XHN_EXPORT OpenGLException : public Exception
{
    DeclareRTTI;
public:
    OpenGLException(const char* file, euint32 line, const char* msg)
        : Exception(file, line, msg)
	{}
};
class XHN_EXPORT ObjectException : public Exception
{
	DeclareRTTI;
public:
	ObjectException(const char* file, euint32 line, const char* msg)
    : Exception(file, line, msg)
	{}
};
class XHN_EXPORT MemoryException : public Exception
{
	DeclareRTTI
public:
	MemoryException(const char* file, euint32 line, const char* msg)
    : Exception(file, line, msg)
	{}
};


/// 函数的输入参数为非法
class XHN_EXPORT FunctionArgumentException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionArgumentException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};
/// 函数执行过程的异常
class XHN_EXPORT FunctionExecutionException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionExecutionException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};
/// 无效操作
class XHN_EXPORT InvalidOperationException : public FunctionExecutionException
{
    DeclareRTTI;
public:
    InvalidOperationException(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// 非预期值异常
class XHN_EXPORT UnexpectedValueException : public FunctionExecutionException
{
    DeclareRTTI;
public:
    UnexpectedValueException(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// 进入不该进入的条件分支
class XHN_EXPORT UndesiredBranch : public FunctionExecutionException
{
	DeclareRTTI;
public:
	UndesiredBranch(const char* file, euint32 line, const char* msg)
	: FunctionExecutionException(file, line, msg)
	{}
};
/// 函数结果不是期望值
class XHN_EXPORT FunctionResultException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionResultException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};
class XHN_EXPORT SubscriptOutOfBounds : public FunctionExecutionException
{
    DeclareRTTI;
public:
    SubscriptOutOfBounds(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// switch case里的枚举值是无效的
class XHN_EXPORT InvalidEnumerationException : public FunctionExecutionException
{
    DeclareRTTI;
public:
    InvalidEnumerationException(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// 对象名已存在
class XHN_EXPORT ObjectNameAlreadyExistedException : public ObjectException
{
    DeclareRTTI;
public:
	ObjectNameAlreadyExistedException(const char* file, euint32 line, const char* msg)
		: ObjectException(file, line, msg)
	{}
};
class XHN_EXPORT ObjectAlreadyExistedException : public ObjectException
{
    DeclareRTTI;
public:
	ObjectAlreadyExistedException(const char* file, euint32 line, const char* msg)
    : ObjectException(file, line, msg)
	{}
};

class XHN_EXPORT ObjectUninitializedException : public ObjectException
{
	DeclareRTTI;
public:
	ObjectUninitializedException(const char* file, euint32 line, const char* msg)
		: ObjectException(file, line, msg)
	{}
};
class XHN_EXPORT ObjectSubscriptOutOfRange : public ObjectException
{
	DeclareRTTI;
public:
	ObjectSubscriptOutOfRange(const char* file, euint32 line, const char* msg)
    : ObjectException(file, line, msg)
	{}
};
class XHN_EXPORT ObjectNotExistedException : public ObjectException
{
    DeclareRTTI;
public:
    ObjectNotExistedException(const char* file, euint32 line, const char* msg)
    : ObjectException(file, line, msg)
	{}
};

/// 无效内存分配异常
class XHN_EXPORT InvalidMemoryAllocException : public MemoryException
{
    DeclareRTTI
public:
	InvalidMemoryAllocException(const char* file, euint32 line, const char* msg)
		: MemoryException(file, line, msg)
	{}
};
class XHN_EXPORT InvalidMemoryFreeException : public MemoryException
{
	DeclareRTTI
public:
	InvalidMemoryFreeException(const char* file, euint32 line, const char* msg)
		: MemoryException(file, line, msg)
	{}
};
#define xhnSTLExce(e, m) throw e(__FILE__, __LINE__, (m))
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
