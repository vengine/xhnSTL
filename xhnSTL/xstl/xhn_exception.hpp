/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

//
//  xhn_exception.hpp
//  ecg
//
//  Created by 徐 海宁 on 13-5-3.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#ifndef ecg_xhn_exception_hpp
#define ecg_xhn_exception_hpp

#include <exception>
#include "rtti.hpp"
#include "elog.h"
/**
#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_MAC
#include "osx_dialog.h"
#endif
#endif
**/
class ExceptionBody;
class Exception : public std::exception
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
class ResourceException : public Exception
{
    DeclareRTTI;
public:
	ResourceException(const char* file, euint32 line, const char* msg)
    : Exception(file, line, msg)
	{}
};
/// 线程异常包括取消线程等一般不catch
class ThreadException : public Exception
{
    DeclareRTTI;
public:
	ThreadException(const char* file, euint32 line, const char* msg)
    : Exception(file, line, msg)
	{}
};
/// 函数异常比较严重，直接退出程序
class FunctionException : public Exception
{
	DeclareRTTI;
public:
	FunctionException(const char* file, euint32 line, const char* msg)
		: Exception(file, line, msg)
	{}
};
/// OpenGL异常只是显示错误，静默式报告错误即可
class OpenGLException : public Exception
{
    DeclareRTTI;
public:
    OpenGLException(const char* file, euint32 line, const char* msg)
        : Exception(file, line, msg)
	{}
};
class ObjectException : public Exception
{
	DeclareRTTI;
public:
	ObjectException(const char* file, euint32 line, const char* msg)
    : Exception(file, line, msg)
	{}
};
class MemoryException : public Exception
{
	DeclareRTTI
public:
	MemoryException(const char* file, euint32 line, const char* msg)
    : Exception(file, line, msg)
	{}
};

class LoadResourceException : public ResourceException
{
    DeclareRTTI;
public:
    LoadResourceException(const char* file, euint32 line, const char* msg)
    : ResourceException(file, line, msg)
    {}
};

class NewResourceException : public ResourceException
{
    DeclareRTTI;
public:
    NewResourceException(const char* file, euint32 line, const char* msg)
    : ResourceException(file, line, msg)
    {}
};

class TextureException : public OpenGLException
{
    DeclareRTTI;
public:
    TextureException(const char* file, euint32 line, const char* msg)
        : OpenGLException(file, line, msg)
    {}
};
class TextureCanNotLockWithNoninternalPixelBuffer : public TextureException
{
    DeclareRTTI;
public:
    TextureCanNotLockWithNoninternalPixelBuffer(const char* file, euint32 line, const char* msg)
        : TextureException(file, line, msg)
    {}
};
class VertexDeclarationLackPositionStreamException : public OpenGLException
{
    DeclareRTTI;
public:
    VertexDeclarationLackPositionStreamException(const char* file, euint32 line, const char* msg)
        : OpenGLException(file, line, msg)
	{}
};
class MaterialException : public OpenGLException
{
    DeclareRTTI;
public:
    MaterialException(const char* file, euint32 line, const char* msg)
        : OpenGLException(file, line, msg)
    {}
};
class MaterialParserException : public MaterialException
{
    DeclareRTTI;
public:
    MaterialParserException(const char* file, euint32 line, const char* msg)
        : MaterialException(file, line, msg)
    {}
};
class MaterialArgumentDoesNotExistException : public MaterialException
{
    DeclareRTTI;
public:
    MaterialArgumentDoesNotExistException(const char* file, euint32 line, const char* msg)
        : MaterialException(file, line, msg)
    {}
};
class MaterialArgumentInvalidFormatException : public MaterialException
{
    DeclareRTTI;
public:
    MaterialArgumentInvalidFormatException(const char* file, euint32 line, const char* msg)
        : MaterialException(file, line, msg)
    {}
};
class SymbolValueInvalidTypeException : public MaterialException
{
    DeclareRTTI;
public:
    SymbolValueInvalidTypeException(const char* file, euint32 line, const char* msg)
    : MaterialException(file, line, msg)
	{}
};
class ShaderObjectInvalidTypeException : public MaterialException
{
    DeclareRTTI;
public:
    ShaderObjectInvalidTypeException(const char* file, euint32 line, const char* msg)
    : MaterialException(file, line, msg)
    {}
};
class QuantityInvalidTypeException : public MaterialException
{
    DeclareRTTI;
public:
    QuantityInvalidTypeException(const char* file, euint32 line, const char* msg)
    : MaterialException(file, line, msg)
	{}
};
class ShaderVectorInvalidAssignmentException : public MaterialException
{
    DeclareRTTI;
public:
    ShaderVectorInvalidAssignmentException(const char* file, euint32 line, const char* msg)
    : MaterialException(file, line, msg)
    {}
};
class ShaderInvalidOperationException : public MaterialException
{
    DeclareRTTI;
public:
    ShaderInvalidOperationException(const char* file, euint32 line, const char* msg)
    : MaterialException(file, line, msg)
	{}
};
class ShaderNodeException : public MaterialException
{
    DeclareRTTI;
public:
    ShaderNodeException(const char* file, euint32 line, const char* msg)
    : MaterialException(file, line, msg)
    {}
};
class ShaderNodeInvalidName : public ShaderNodeException
{
    DeclareRTTI;
public:
    ShaderNodeInvalidName(const char* file, euint32 line, const char* msg)
    : ShaderNodeException(file, line, msg)
    {}
};
/// 函数的输入参数为非法
class FunctionArgumentException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionArgumentException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};
/// 函数执行过程的异常
class FunctionExecutionException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionExecutionException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};
/// 无效操作
class InvalidOperationException : public FunctionExecutionException
{
    DeclareRTTI;
public:
    InvalidOperationException(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// 非预期值异常
class UnexpectedValueException : public FunctionExecutionException
{
    DeclareRTTI;
public:
    UnexpectedValueException(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// 进入不该进入的条件分支
class UndesiredBranch : public FunctionExecutionException
{
	DeclareRTTI;
public:
	UndesiredBranch(const char* file, euint32 line, const char* msg)
	: FunctionExecutionException(file, line, msg)
	{}
};
/// 函数结果不是期望值
class FunctionResultException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionResultException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};
class SubscriptOutOfBounds : public FunctionExecutionException
{
    DeclareRTTI;
public:
    SubscriptOutOfBounds(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// switch case里的枚举值是无效的
class InvalidEnumerationException : public FunctionExecutionException
{
    DeclareRTTI;
public:
    InvalidEnumerationException(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// 对象名已存在
class ObjectNameAlreadyExistedException : public ObjectException
{
    DeclareRTTI;
public:
	ObjectNameAlreadyExistedException(const char* file, euint32 line, const char* msg)
		: ObjectException(file, line, msg)
	{}
};
class ObjectAlreadyExistedException : public ObjectException
{
    DeclareRTTI;
public:
	ObjectAlreadyExistedException(const char* file, euint32 line, const char* msg)
    : ObjectException(file, line, msg)
	{}
};
class ResourceGroupNotExistedException : public ObjectException
{
    DeclareRTTI;
public:
    ResourceGroupNotExistedException(const char* file, euint32 line, const char* msg)
    : ObjectException(file, line, msg)
	{}
};
class ObjectUninitializedException : public ObjectException
{
	DeclareRTTI;
public:
	ObjectUninitializedException(const char* file, euint32 line, const char* msg)
		: ObjectException(file, line, msg)
	{}
};
class ObjectSubscriptOutOfRange : public ObjectException
{
	DeclareRTTI;
public:
	ObjectSubscriptOutOfRange(const char* file, euint32 line, const char* msg)
    : ObjectException(file, line, msg)
	{}
};
class ObjectNotExistedException : public ObjectException
{
    DeclareRTTI;
public:
    ObjectNotExistedException(const char* file, euint32 line, const char* msg)
    : ObjectException(file, line, msg)
	{}
};

/// 无效内存分配异常
class InvalidMemoryAllocException : public MemoryException
{
    DeclareRTTI
public:
	InvalidMemoryAllocException(const char* file, euint32 line, const char* msg)
		: MemoryException(file, line, msg)
	{}
};
class InvalidMemoryFreeException : public MemoryException
{
	DeclareRTTI
public:
	InvalidMemoryFreeException(const char* file, euint32 line, const char* msg)
		: MemoryException(file, line, msg)
	{}
};
#define VEngineExce(e, m) throw e(__FILE__, __LINE__, (m))
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