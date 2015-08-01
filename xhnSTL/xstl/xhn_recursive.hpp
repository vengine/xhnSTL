//
//  xhn_recursive.hpp
//  VEngine
//
//  Created by 徐海宁 on 14/12/25.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef VEngine_xhn_recursive_hpp
#define VEngine_xhn_recursive_hpp

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
