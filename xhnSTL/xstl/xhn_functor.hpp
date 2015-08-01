/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef FUNCTOR_H
#define FUNCTOR_H
#include "common.h"
#include "etypes.h"

namespace xhn
{
    class FunctionBase
    {
    public:
        virtual ~FunctionBase() {}
        virtual int NumberParameters() = 0;
    };
    
    /// 零个参数
    
    template<typename _ReturnType>
    class FunctionBase0Parameter : public FunctionBase
    {
    public:
        typedef _ReturnType(*PtrFuncType)(void);
    public:
        virtual PtrFuncType GetFunc() = 0;
        virtual _ReturnType operator()() = 0;
        virtual int NumberParameters() {
            return 0;
        }
    };
    
    template<typename _ReturnType, typename _Class>
    class MemberFunction0Parameter : public FunctionBase0Parameter<_ReturnType>
    {
    public:
        typedef _ReturnType(_Class::*MemFuncType)(void);
        
        MemFuncType m_pfunc;
        _Class* m_obj;
        
        MemberFunction0Parameter(MemFuncType func, _Class* obj)
        {
            m_pfunc = func;
            m_obj = obj;
        }
        
        virtual typename FunctionBase0Parameter<_ReturnType>::PtrFuncType GetFunc()
        {
            return (typename FunctionBase0Parameter<_ReturnType>::PtrFuncType)(*(long*)&m_pfunc);
        }
        
        virtual _ReturnType operator()()
        {
            return (m_obj->*m_pfunc)();
        }
    };
    
    template<typename _ReturnType>
    class CommonFunction0Parameter : public FunctionBase0Parameter<_ReturnType>
    {
    public:
        typename FunctionBase0Parameter<_ReturnType>::PtrFuncType m_pfunc;
        
        CommonFunction0Parameter(typename FunctionBase0Parameter<_ReturnType>::PtrFuncType pf)
        {
            m_pfunc = pf;
        }
        
        virtual typename FunctionBase0Parameter<_ReturnType>::PtrFuncType GetFunc()
        {
            return m_pfunc;
        }
        
        virtual _ReturnType operator()()
        {
            return (*m_pfunc)();
        }
    };
    
    /// 一个参数
    
    template<typename _ReturnType, typename _T1>
    class FunctionBase1Parameter : public FunctionBase
    {
    public:
        typedef _ReturnType(*PtrFuncType)(_T1);
    public:
        virtual PtrFuncType GetFunc() = 0;
        virtual _ReturnType operator()(_T1 arg1) = 0;
        virtual int NumberParameters() {
            return 1;
        }
    };
    
    template<typename _ReturnType, typename _Class, typename _T1>
    class MemberFunction1Parameter : public FunctionBase1Parameter<_ReturnType, _T1>
    {
    public:
        typedef _ReturnType(_Class::*MemFuncType)(_T1);
        
        MemFuncType m_pfunc;
        _Class* m_obj;
        
        MemberFunction1Parameter(MemFuncType func, _Class* obj)
        {
            m_pfunc = func;
            m_obj = obj;
        }
        
        virtual typename FunctionBase1Parameter<_ReturnType, _T1>::PtrFuncType GetFunc()
        {
            return (typename FunctionBase1Parameter<_ReturnType, _T1>::PtrFuncType)(*(long*)&m_pfunc);
        }
        
        virtual _ReturnType operator()(_T1 arg1)
        {
            return (m_obj->*m_pfunc)(arg1);
        }
    };
    
    template<typename _ReturnType, typename _T1>
    class CommonFunction1Parameter : public FunctionBase1Parameter<_ReturnType, _T1>
    {
    public:
        typename FunctionBase1Parameter<_ReturnType, _T1>::PtrFuncType m_pfunc;
        
        CommonFunction1Parameter(typename FunctionBase1Parameter<_ReturnType, _T1>::PtrFuncType pf)
        {
            m_pfunc = pf;
        }
        
        virtual typename FunctionBase1Parameter<_ReturnType, _T1>::PtrFuncType GetFunc()
        {
            return m_pfunc;
        }
        
        virtual _ReturnType operator()(_T1 arg1)
        {
            return (*m_pfunc)(arg1);
        }
    };
    
    /// 两个参数
    
    template<typename _ReturnType, typename _T1, typename _T2>
    class FunctionBase2Parameters : public FunctionBase
    {
    public:
        typedef _ReturnType(*PtrFuncType)(_T1, _T2);
    public:
        virtual ~FunctionBase2Parameters() {}
        virtual PtrFuncType GetFunc() = 0;
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2) = 0;
        virtual int NumberParameters() {
            return 2;
        }
    };
    
    template<typename _ReturnType, typename _Class, typename _T1, typename _T2>
    class MemberFunction2Parameters : public FunctionBase2Parameters<_ReturnType, _T1, _T2>
    {
    public:
        typedef _ReturnType(_Class::*MemFuncType)(_T1, _T2);
        
        MemFuncType m_pfunc;
        _Class* m_obj;
        
        MemberFunction2Parameters(MemFuncType func, _Class* obj)
        {
            m_pfunc = func;
            m_obj = obj;
        }
        
        virtual typename FunctionBase2Parameters<_ReturnType, _T1, _T2>::PtrFuncType GetFunc()
        {
            return (typename FunctionBase2Parameters<_ReturnType, _T1, _T2>::PtrFuncType)(*(long*)&m_pfunc);
        }
        
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2)
        {
            return (m_obj->*m_pfunc)(arg1, arg2);
        }
    };
    
    template<typename _ReturnType, typename _T1, typename _T2>
    class CommonFunction2Parameters : public FunctionBase2Parameters<_ReturnType, _T1, _T2>
    {
    public:
        typename FunctionBase2Parameters<_ReturnType, _T1, _T2>::PtrFuncType m_pfunc;
        
        CommonFunction2Parameters(typename FunctionBase2Parameters<_ReturnType, _T1, _T2>::PtrFuncType pf)
        {
            m_pfunc = pf;
        }
        
        virtual typename FunctionBase2Parameters<_ReturnType, _T1, _T2>::PtrFuncType GetFunc()
        {
            return m_pfunc;
        }
        
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2)
        {
            return (*m_pfunc)(arg1, arg2);
        }
    };
    
    /// 三个参数
    
    template<typename _ReturnType, typename _T1, typename _T2, typename _T3>
    class FunctionBase3Parameters : public FunctionBase
    {
    public:
        typedef _ReturnType(*PtrFuncType)(_T1, _T2, _T3);
    public:
        virtual ~FunctionBase3Parameters() {}
        virtual PtrFuncType GetFunc() = 0;
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2, _T3 arg3) = 0;
        virtual int NumberParameters() {
            return 3;
        }
    };
    
    template<typename _ReturnType, typename _Class, typename _T1, typename _T2, typename _T3>
    class MemberFunction3Parameters : public FunctionBase3Parameters<_ReturnType, _T1, _T2, _T3>
    {
    public:
        typedef _ReturnType(_Class::*MemFuncType)(_T1, _T2, _T3);
        
        MemFuncType m_pfunc;
        _Class* m_obj;
        
        MemberFunction3Parameters(MemFuncType func, _Class* obj)
        {
            m_pfunc = func;
            m_obj = obj;
        }
        
        virtual typename FunctionBase3Parameters<_ReturnType, _T1, _T2, _T3>::PtrFuncType GetFunc()
        {
            return (typename FunctionBase3Parameters<_ReturnType, _T1, _T2, _T3>::PtrFuncType)(*(long*)&m_pfunc);
        }
        
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2, _T3 arg3)
        {
            return (m_obj->*m_pfunc)(arg1, arg2, arg3);
        }
    };
    
    template<typename _ReturnType, typename _T1, typename _T2, typename _T3>
    class CommonFunction3Parameters : public FunctionBase3Parameters<_ReturnType, _T1, _T2, _T3>
    {
    public:
        typename FunctionBase3Parameters<_ReturnType, _T1, _T2, _T3>::PtrFuncType m_pfunc;
        
        CommonFunction3Parameters(typename FunctionBase3Parameters<_ReturnType, _T1, _T2, _T3>::PtrFuncType pf)
        {
            m_pfunc = pf;
        }
        
        virtual typename FunctionBase3Parameters<_ReturnType, _T1, _T2, _T3>::PtrFuncType GetFunc()
        {
            return m_pfunc;
        }
        
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2, _T3 arg3)
        {
            return (*m_pfunc)(arg1, arg2, arg3);
        }
    };
    
    /// 四个参数
    
    template<typename _ReturnType, typename _T1, typename _T2, typename _T3, typename _T4>
    class FunctionBase4Parameters : public FunctionBase
    {
    public:
        typedef _ReturnType(*PtrFuncType)(_T1, _T2, _T3, _T4);
    public:
        virtual ~FunctionBase4Parameters() {}
        virtual PtrFuncType GetFunc() = 0;
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2, _T3 arg3, _T4 arg4) = 0;
        virtual int NumberParameters() {
            return 4;
        }
    };
    
    template<typename _ReturnType, typename _Class, typename _T1, typename _T2, typename _T3, typename _T4>
    class MemberFunction4Parameters : public FunctionBase4Parameters<_ReturnType, _T1, _T2, _T3, _T4>
    {
    public:
        typedef _ReturnType(_Class::*MemFuncType)(_T1, _T2, _T3);
        
        MemFuncType m_pfunc;
        _Class* m_obj;
        
        MemberFunction4Parameters(MemFuncType func, _Class* obj)
        {
            m_pfunc = func;
            m_obj = obj;
        }
        
        virtual typename FunctionBase4Parameters<_ReturnType, _T1, _T2, _T3, _T4>::PtrFuncType GetFunc()
        {
            return (typename FunctionBase4Parameters<_ReturnType, _T1, _T2, _T3, _T4>::PtrFuncType)(*(long*)&m_pfunc);
        }
        
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2, _T3 arg3, _T4 arg4)
        {
            return (m_obj->*m_pfunc)(arg1, arg2, arg3, arg4);
        }
    };
    
    template<typename _ReturnType, typename _T1, typename _T2, typename _T3, typename _T4>
    class CommonFunction4Parameters : public FunctionBase4Parameters<_ReturnType, _T1, _T2, _T3, _T4>
    {
    public:
        typename FunctionBase4Parameters<_ReturnType, _T1, _T2, _T3, _T4>::PtrFuncType m_pfunc;
        
        CommonFunction4Parameters(typename FunctionBase4Parameters<_ReturnType, _T1, _T2, _T3, _T4>::PtrFuncType pf)
        {
            m_pfunc = pf;
        }
        
        virtual typename FunctionBase4Parameters<_ReturnType, _T1, _T2, _T3, _T4>::PtrFuncType GetFunc()
        {
            return m_pfunc;
        }
        
        virtual _ReturnType operator()(_T1 arg1, _T2 arg2, _T3 arg3, _T4 arg4)
        {
            return (*m_pfunc)(arg1, arg2, arg3, arg4);
        }
    };
    
    ///
    
    template<typename _ReturnType>
    class Functor
    {
    public:
        mutable FunctionBase* m_pfunc;
    public:
        Functor() : m_pfunc(NULL)
        {}
        ~Functor()
        {
            if (m_pfunc != NULL)
            {
                delete m_pfunc;
                m_pfunc = NULL;
            }
        }
        
        template<typename _T1, typename _T2, typename _T3, typename _T4>
        _ReturnType operator()(_T1 arg1, _T2 arg2, _T3 arg3, _T4 arg4)
        {
            return (*(FunctionBase4Parameters<_ReturnType, _T1, _T2, _T3, _T4>*)m_pfunc)(arg1, arg2, arg3, arg4);
        }
        template<typename _T1, typename _T2, typename _T3>
        _ReturnType operator()(_T1 arg1, _T2 arg2, _T3 arg3)
        {
            return (*(FunctionBase3Parameters<_ReturnType, _T1, _T2, _T3>*)m_pfunc)(arg1, arg2, arg3);
        }
        template<typename _T1, typename _T2>
        _ReturnType operator()(_T1 arg1, _T2 arg2)
        {
            return (*(FunctionBase2Parameters<_ReturnType, _T1, _T2>*)m_pfunc)(arg1, arg2);
        }
        template<typename _T1>
        _ReturnType operator()(_T1 arg1)
        {
            return (*(FunctionBase1Parameter<_ReturnType, _T1>*)m_pfunc)(arg1);
        }
        _ReturnType operator()()
        {
            return (*(FunctionBase0Parameter<_ReturnType>*)m_pfunc)();
        }
        
        Functor& operator=(const Functor& f)
        {
            m_pfunc = f.m_pfunc;
            f.m_pfunc = NULL;
            return *this;
        }
    };
    
    template<typename _ReturnType, typename _Class, typename _T1, typename _T2, typename _T3, typename _T4>
    Functor<_ReturnType> bind(_ReturnType(_Class::*pMemFunc)(_T1, _T2, _T3, _T4), _Class* obj)
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new MemberFunction4Parameters<_ReturnType, _Class, _T1, _T2, _T3, _T4>(pMemFunc, obj);
        return ret;
    }
    
    template<typename _ReturnType, typename _T1, typename _T2, typename _T3, typename _T4>
    Functor<_ReturnType> bind(_ReturnType(*pFunc)(_T1, _T2, _T3, _T4))
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new CommonFunction4Parameters<_ReturnType, _T1, _T2, _T3, _T4>(pFunc);
        return ret;
    }
    
    template<typename _ReturnType, typename _Class, typename _T1, typename _T2, typename _T3>
    Functor<_ReturnType> bind(_ReturnType(_Class::*pMemFunc)(_T1, _T2, _T3), _Class* obj)
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new MemberFunction3Parameters<_ReturnType, _Class, _T1, _T2, _T3>(pMemFunc, obj);
        return ret;
    }
    
    template<typename _ReturnType, typename _T1, typename _T2, typename _T3>
    Functor<_ReturnType> bind(_ReturnType(*pFunc)(_T1, _T2, _T3))
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new CommonFunction3Parameters<_ReturnType, _T1, _T2, _T3>(pFunc);
        return ret;
    }
    
    template<typename _ReturnType, typename _Class, typename _T1, typename _T2>
    Functor<_ReturnType> bind(_ReturnType(_Class::*pMemFunc)(_T1, _T2), _Class* obj)
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new MemberFunction2Parameters<_ReturnType, _Class, _T1, _T2>(pMemFunc, obj);
        return ret;
    }
    
    template<typename _ReturnType, typename _T1, typename _T2>
    Functor<_ReturnType> bind(_ReturnType(*pFunc)(_T1, _T2))
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new CommonFunction2Parameters<_ReturnType, _T1, _T2>(pFunc);
        return ret;
    }
    
    template<typename _ReturnType, typename _Class, typename _T1>
    Functor<_ReturnType> bind(_ReturnType(_Class::*pMemFunc)(_T1), _Class* obj)
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new MemberFunction1Parameter<_ReturnType, _Class, _T1>(pMemFunc, obj);
        return ret;
    }
    
    template<typename _ReturnType, typename _T1>
    Functor<_ReturnType> bind(_ReturnType(*pFunc)(_T1))
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new CommonFunction1Parameter<_ReturnType, _T1>(pFunc);
        return ret;
    }
    
    template<typename _ReturnType, typename _Class>
    Functor<_ReturnType> bind(_ReturnType(_Class::*pMemFunc)(), _Class* obj)
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new MemberFunction0Parameter<_ReturnType, _Class>(pMemFunc, obj);
        return ret;
    }
    
    template<typename _ReturnType>
    Functor<_ReturnType> bind(_ReturnType(*pFunc)())
    {
        Functor<_ReturnType> ret;
        ret.m_pfunc = new CommonFunction0Parameter<_ReturnType>(pFunc);
        return ret;
    }
}

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
