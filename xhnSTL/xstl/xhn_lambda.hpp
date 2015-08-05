/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef LAMBDA_H
#define LAMBDA_H
#include "common.h"
#include "etypes.h"
#include "elog.h"
#include "eassert.h"

namespace xhn
{
    // LambdaExecutor is an internal class that adds the ability to execute to
    // Lambdas.  This functionality is separated because it is the only thing
    // that needed to be specialized (by return type).
    
    // generateExecutor or receiveExecutor must be called after constructing,
    // before use
    template<typename T>
    class LambdaExecutor {};
    
    template <typename Out, typename... In>
    class LambdaExecutor<Out(In...)> {
    public:
        Out operator()(In ... in)
        {
            EAssert(lambda != nullptr, "lambda is nullptr");
            return executeLambda(lambda, in...);
        }
        
    protected:
        LambdaExecutor(void *&lambda) : lambda(lambda) {}
        
        ~LambdaExecutor() {}
        
        template <typename T> void generateExecutor(T const &lambda)
        {
            executeLambda = [](void *lambda, In... arguments) -> Out
            {
                return ((T *)lambda)->operator()(arguments...);
            };
        }
        
        void receiveExecutor(LambdaExecutor<Out(In...)> const &other)
        {
            executeLambda = other.executeLambda;
        }
        
    private:
        void *&lambda;
        Out (*executeLambda)(void *, In...);
    };
    
    template <typename... In>
    class LambdaExecutor<void(In...)> {
    public:
        void operator()(In ... in)
        {
            EAssert(lambda != nullptr, "lambda is nullptr");
            executeLambda(lambda, in...);
        }
        
    protected:
        LambdaExecutor(void *&lambda) : lambda(lambda) {}
        
        ~LambdaExecutor() {}
        
        template <typename T> void generateExecutor(T const &lambda)
        {
            executeLambda = [](void *lambda, In... arguments)
            {
                return ((T *)lambda)->operator()(arguments...);
            };
        }
        
        void receiveExecutor(LambdaExecutor<void(In...)> const &other)
        {
            executeLambda = other.executeLambda;
        }
        
    private:
        void *&lambda;
        void (*executeLambda)(void *, In...);
    };
    
    // Lambda contains most of the lambda management code and can be used
    // directly in external code.
    template <typename T>
    class Lambda {};
    
    template <typename Out, typename ...In>
    class Lambda<Out(In...)> : public LambdaExecutor<Out(In...)> {
    public:
        Lambda()
        : LambdaExecutor<Out(In...)>(lambda)
        , lambda(nullptr)
        , deleteLambda(nullptr)
        , copyLambda(nullptr)
        {
        }
        
        Lambda(Lambda<Out(In...)> const &other)
        : LambdaExecutor<Out(In...)>(lambda)
        , lambda(other.copyLambda ? other.copyLambda(other.lambda) : nullptr)
        , deleteLambda(other.deleteLambda)
        , copyLambda(other.copyLambda)
        {
            this->receiveExecutor(other);
        }
        
        template<typename T>
        Lambda(T const &lambda)
        : LambdaExecutor<Out(In...)>(this->lambda)
        , lambda(nullptr)
        {
            // Copy should set all variables
            copy(lambda);
        }
        
        ~Lambda()
        {
            if (lambda != nullptr && deleteLambda != nullptr) deleteLambda(lambda);
        }
        
        Lambda<Out(In...)> &operator =(Lambda<Out(In...)> const &other)
        {
            if (this->lambda != nullptr && this->deleteLambda != nullptr) deleteLambda(this->lambda);
            this->lambda = other.copyLambda ? other.copyLambda(other.lambda) : nullptr;
            this->receiveExecutor(other);
            this->deleteLambda = other.deleteLambda;
            this->copyLambda = other.copyLambda;
            return *this;
        }
        
        template<typename T> Lambda<Out(In...)> &operator =(T const &lambda)
        {
            copy(lambda);
            return *this;
        }
        
        operator bool() const
        {
            return lambda != nullptr;
        }
        
    private:
        template<typename T>
        void copy(T const &lambda)
        {
            if (this->lambda != nullptr) deleteLambda(this->lambda);
            this->lambda = new T(lambda);
            
            this->generateExecutor(lambda);
            
            deleteLambda = [](void *lambda)
            {
                delete (T *)lambda;
            };
            
            copyLambda = [](void *lambda) -> void *
            {
                return lambda ? new T(*(T *)lambda) : nullptr;
            };
        }
        
        void *lambda;
        void (*deleteLambda)(void *);
        void *(*copyLambda)(void *);
    };
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
