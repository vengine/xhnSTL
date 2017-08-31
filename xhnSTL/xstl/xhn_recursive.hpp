/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_RESURSIVE_HPP
#define XHN_RESURSIVE_HPP

#ifdef __cplusplus

#include "xhn_vector.hpp"
#include "xhn_lambda.hpp"
namespace xhn
{
    template <typename WORK>
    class recursive
    {
    public:
        typedef vector<WORK> workflow;
    public:
        workflow m_workflow0;
        workflow m_workflow1;
        workflow* m_current_workflow;
        workflow* m_next_workflow;
        Lambda<void (workflow*, workflow*)> m_workflow_proc;
    public:
        recursive()
        : m_current_workflow(nullptr)
        , m_next_workflow(nullptr)
        {}
        recursive(const Lambda<void (workflow* current_workflow, workflow* next_workflow)>& proc)
        : m_current_workflow(nullptr)
        , m_next_workflow(nullptr)
        , m_workflow_proc(proc)
        {}
        void set_proc(const Lambda<void (workflow* current_workflow, workflow* next_workflow)>& proc) {
            m_workflow_proc = proc;
        }
        void add_work(WORK& w) {
            m_workflow0.push_back(w);
        }
        void work() {
            m_current_workflow = &m_workflow0;
            m_next_workflow = &m_workflow1;
            while (m_current_workflow->size()) {
                m_workflow_proc(m_current_workflow, m_next_workflow);
                workflow* tmp = m_next_workflow;
                m_current_workflow->clear();
                m_next_workflow = m_current_workflow;
                m_current_workflow = tmp;
            }
        }
        void clear() {
            m_workflow0.clear();
            m_workflow1.clear();
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
